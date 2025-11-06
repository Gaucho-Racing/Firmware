FROM ubuntu:20.04

# Set default ARGs for ARM toolchain version and release
ARG TOOLCHAIN_VERSION=12.2.rel1
ARG TOOLCHAIN_BASE_URL=https://developer.arm.com/-/media/Files/downloads/gnu/${TOOLCHAIN_VERSION}/binrel
ARG TOOLCHAIN_NAME_BASE=arm-gnu-toolchain-${TOOLCHAIN_VERSION}

# prevent `apt install` from prompting for user input
ENV DEBIAN_FRONTEND=noninteractive

# standard dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    git \
    wget \
    vim \
    nano \
    python3 \
    python3-pip \
    ninja-build \
    libncurses5 \
    software-properties-common \
    ca-certificates \
    gpg

# latest CMAKE
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null \
    && echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    && apt-get update \
    && apt-get install -y cmake

# Install OpenOCD 0.12.0 from xPack
RUN ARCH=$(uname -m) && \
    case "$ARCH" in \
    x86_64) OPENOCD_PLATFORM="linux-x64" ;; \
    aarch64) OPENOCD_PLATFORM="linux-arm64" ;; \
    armv7l) OPENOCD_PLATFORM="linux-arm" ;; \
    *) echo "Unsupported architecture: $ARCH" && exit 1 ;; \
    esac && \
    OPENOCD_VERSION="0.12.0-2" && \
    OPENOCD_BASENAME="xpack-openocd-${OPENOCD_VERSION}" && \
    echo "Downloading OpenOCD for architecture: $ARCH (${OPENOCD_PLATFORM})" && \
    wget -q "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v${OPENOCD_VERSION}/${OPENOCD_BASENAME}-${OPENOCD_PLATFORM}.tar.gz" -O /tmp/openocd.tar.gz && \
    tar -xzf /tmp/openocd.tar.gz -C /opt && \
    ln -sf "/opt/${OPENOCD_BASENAME}/bin/openocd" /usr/local/bin/openocd && \
    rm /tmp/openocd.tar.gz && \
    echo "OpenOCD ${OPENOCD_VERSION} installed successfully"

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

# cleanup apt cache
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

ENV PATH="/usr/local/arm-gnu-toolchain/bin:${PATH}"
