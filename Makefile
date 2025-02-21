# Use PowerShell commands for Windows
ifeq ($(OS),Windows_NT)
    FIND := powershell -Command "Get-ChildItem -Recurse -Filter"
    MKDIR := powershell -Command "New-Item -ItemType Directory -Force -Path"
    RM := powershell -Command "Remove-Item -Recurse -Force -ErrorAction SilentlyContinue"
else
    FIND := find
    MKDIR := mkdir -p
    RM := rm -rf
endif

# Source files
kernel_source_files := $(wildcard src/impl/kernel/*.c)
kernel_object_files := $(patsubst src/impl/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

x86_64_c_source_files := $(wildcard src/impl/x86_64/*.c)
x86_64_c_object_files := $(patsubst src/impl/x86_64/%.c, build/x86_64/%.o, $(x86_64_c_source_files))

x86_64_asm_source_files := $(wildcard src/impl/x86_64/*.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

boot_asm_source_files := $(wildcard src/impl/x86_64/boot/*.asm)
boot_asm_object_files := $(patsubst src/impl/x86_64/boot/%.asm, build/x86_64/boot/%.o, $(boot_asm_source_files))

driver_source_files := $(wildcard src/impl/drivers/*/*.c)
driver_object_files := $(patsubst src/impl/drivers/%.c, build/drivers/%.o, $(driver_source_files))

x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files) $(boot_asm_object_files)

# Test files
test_source_files := $(wildcard tests/*.c)
test_object_files := $(patsubst tests/%.c, build/tests/%.o, $(test_source_files))

# Toolchain paths for Windows
ifeq ($(OS),Windows_NT)
    CC := x86_64-elf-gcc.exe
    LD := x86_64-elf-ld.exe
    ASM := nasm.exe
else
    CC := x86_64-elf-gcc
    LD := x86_64-elf-ld
    ASM := nasm
endif

# Compiler flags
CFLAGS := -I src/intf \
          -ffreestanding \
          -fno-stack-protector \
          -mno-red-zone \
          -mno-mmx \
          -mno-sse \
          -mno-sse2 \
          -fno-builtin \
          -fno-pic \
          -O2 \
          -Wall \
          -Wextra \
          -fasm \
          -masm=intel \
          -nostdinc \
          -std=gnu11

# Test flags (add debug info for testing)
TEST_CFLAGS := $(CFLAGS) -g -DTEST_MODE

# Assembler flags
ASMFLAGS := -f elf64

# Linker flags
LDFLAGS := -n -T targets/x86_64/linker.ld

# Build rules
$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(x86_64_c_object_files): build/x86_64/%.o : src/impl/x86_64/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	$(MKDIR) $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(boot_asm_object_files): build/x86_64/boot/%.o : src/impl/x86_64/boot/%.asm
	$(MKDIR) $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(driver_object_files): build/drivers/%.o : src/impl/drivers/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Test build rules
$(test_object_files): build/tests/%.o : tests/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files) $(driver_object_files)
	$(MKDIR) dist/x86_64
	$(MKDIR) targets/x86_64/iso/boot
	$(LD) $(LDFLAGS) -o dist/x86_64/kernel.bin $^
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

.PHONY: test-x86_64
test-x86_64: $(kernel_object_files) $(x86_64_object_files) $(driver_object_files) $(test_object_files)
	$(MKDIR) dist/x86_64
	$(MKDIR) targets/x86_64/iso/boot
	$(LD) $(LDFLAGS) -o dist/x86_64/kernel_test.bin $^
	cp dist/x86_64/kernel_test.bin targets/x86_64/iso/boot/kernel.bin
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel_test.iso targets/x86_64/iso

.PHONY: test
test: test-x86_64
	@echo "Test build complete. Run the kernel_test.iso in QEMU to execute tests."
	@echo "Run with: qemu-system-x86_64 -cdrom dist/x86_64/kernel_test.iso"

.PHONY: external-test
external-test:
	$(MKDIR) test_output
	chmod +x tests/external_test.sh
	./tests/external_test.sh

.PHONY: clean
clean:
	$(RM) build
	$(RM) dist
	$(RM) test_output


