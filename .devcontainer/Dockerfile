FROM ubuntu:20.04

# Set default ARGs for ARM toolchain version and release
ARG TOOLCHAIN_VERSION=12.2.rel1
ARG TOOLCHAIN_BASE_URL=https://developer.arm.com/-/media/Files/downloads/gnu/${TOOLCHAIN_VERSION}/binrel
ARG TOOLCHAIN_NAME_BASE=arm-gnu-toolchain-${TOOLCHAIN_VERSION}

# prevent `apt install` from prompting for user input
ENV DEBIAN_FRONTEND=noninteractive

# Install base dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    git \
    wget \
    vim \
    nano \
    python3 \
    python3-pip \
    cmake \
    ninja-build \
    openocd \
    libncurses5 \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# arm-none-eabi-gdb requires python3.8
RUN add-apt-repository ppa:deadsnakes/ppa -y && apt-get update && apt-get install -y python3.8

# Detect architecture and set the correct toolchain URL
RUN ARCH=$(uname -m) && \
    case "$ARCH" in \
    x86_64) TOOLCHAIN_URL="${TOOLCHAIN_BASE_URL}/${TOOLCHAIN_NAME_BASE}-x86_64-arm-none-eabi.tar.xz" ;; \
    aarch64) TOOLCHAIN_URL="${TOOLCHAIN_BASE_URL}/${TOOLCHAIN_NAME_BASE}-aarch64-arm-none-eabi.tar.xz" ;; \
    *) echo "Unsupported architecture: $ARCH" && exit 1 ;; \
    esac && \
    echo "Downloading ARM toolchain for architecture: $ARCH" && \
    wget -O arm-gnu-toolchain.tar.xz "$TOOLCHAIN_URL" && \
    mkdir -p /usr/local/arm-gnu-toolchain && \
    tar -xf arm-gnu-toolchain.tar.xz -C /usr/local/arm-gnu-toolchain --strip-components 1 && \
    rm arm-gnu-toolchain.tar.xz

ENV PATH="/usr/local/arm-gnu-toolchain/bin:${PATH}"
