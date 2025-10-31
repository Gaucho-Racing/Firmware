FROM ubuntu:latest

# If we ever need to change the version just update this to the new URL
ARG arm_toolchain_url=https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz?rev=7bd049b7a3034e64885fa1a71c12f91d&hash=2C60D7D4E432953DB65C4AA2E7129304F9CD05BF

# Install base dependencies
# For libncurses
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
    libncurses6 \
    software-properties-common


# arm-none-eabi-gdb requires libncurses5, but that's not available by default
# libncurses6 is similar enough to libncurses5 for our purposes
# so we'll just pretend that libncurses6 is libncurses5
RUN ln -s /lib/x86_64-linux-gnu/libncursesw.so.6 /usr/lib/libncursesw.so.5

# arm-none-eabi-gdb requires python3.8
RUN add-apt-repository ppa:deadsnakes/ppa -y
RUN apt install python3.8 -y

# Install a specific version of the ARM GCC Toolchain
RUN wget "${arm_toolchain_url}" -O arm-gnu-toolchain.tar.xz
RUN mkdir arm-gnu-toolchain && tar -xf arm-gnu-toolchain.tar.xz -C arm-gnu-toolchain --strip-components 1 
RUN mv arm-gnu-toolchain /usr/local/arm-gnu-toolchain
RUN rm arm-gnu-toolchain.tar.xz
ENV PATH="/usr/local/arm-gnu-toolchain/bin:${PATH}"
