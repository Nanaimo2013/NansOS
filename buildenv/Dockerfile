FROM randomdude/gcc-cross-x86_64-elf:latest

LABEL org.opencontainers.image.title="NansOS Build Environment"
LABEL org.opencontainers.image.description="Development environment for building NansOS"
LABEL org.opencontainers.image.authors="NansStudios"
LABEL org.opencontainers.image.source="https://github.com/Nanaimo2013/NansOS"
LABEL org.opencontainers.image.licenses="MIT"

# Install required packages
RUN set -eux; \
    apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        nasm \
        xorriso \
        grub-pc-bin \
        grub-common \
        gcc-multilib \
        g++-multilib \
        build-essential \
        binutils \
        libc6-dev \
        gcc-x86-64-linux-gnu \
        clang \
        llvm \
        make; \
    apt-get clean; \
    rm -rf /var/lib/apt/lists/*

# Create and set permissions for workspace
RUN mkdir -p /root/env && chmod 777 /root/env
WORKDIR /root/env
VOLUME /root/env

# Set environment variables for GCC
ENV CC=x86_64-elf-gcc \
    CXX=x86_64-elf-g++ \
    LD=x86_64-elf-ld \
    AR=x86_64-elf-ar \
    CFLAGS="-ffreestanding -O2 -Wall -Wextra -D__GNUC__ -fasm -masm=intel" \
    LDFLAGS="-nostdlib"

# Default command
CMD ["/bin/bash"]