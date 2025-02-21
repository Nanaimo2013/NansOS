FROM randomdude/gcc-cross-x86_64-elf:latest

LABEL org.opencontainers.image.title="NansOS Build Environment"
LABEL org.opencontainers.image.description="Development environment for building NansOS"
LABEL org.opencontainers.image.authors="NansStudios"
LABEL org.opencontainers.image.source="https://github.com/Nanaimo2013/NansOS"
LABEL org.opencontainers.image.licenses="MIT"

# Install required packages
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    && rm -rf /var/lib/apt/lists/*

# Set up workspace
VOLUME /root/env
WORKDIR /root/env

# Default command
CMD ["/bin/bash"]