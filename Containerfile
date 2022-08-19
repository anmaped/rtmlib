FROM debian:11 as qemu-build-stage

RUN apt update && apt install -y \
    build-essential \
    git \
    python3 \
    pkg-config \
    ninja-build \
    libglib2.0-dev \
    libpixman-1-dev

RUN cd /tmp && \
    git clone --depth 1 --branch v6.1.0 https://gitlab.com/qemu-project/qemu.git qemu-riscv-src && \
    cd qemu-riscv-src && \
    ./configure --prefix=/opt/qemu --target-list=riscv64-softmmu && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/riscv-gnu-toolchain    


FROM debian:11 as gcc-riscv-build-stage

RUN apt update && apt install -y \
    build-essential \
    git \
    curl \
    gawk \
    bison \
    flex \
    texinfo \
    zlib1g-dev \
    libexpat-dev

RUN git clone -b 2021.04.23 --depth 1 https://github.com/riscv/riscv-gnu-toolchain /tmp/riscv-gnu-toolchain && \
    cd /tmp/riscv-gnu-toolchain  && \
    git submodule init && \
    git submodule update --depth 1

RUN mkdir -p /opt/gcc-riscv && \
    cd /tmp/riscv-gnu-toolchain && \
    ./configure --prefix=/opt/gcc-riscv --enable-gdb --with-cmodel=medany --with-libatomic && \
    make -j $(nproc) && \
    rm -rf /tmp/riscv-gnu-toolchain    


FROM debian:11

RUN apt update
RUN apt install -y \
    build-essential \
    wget \
    git

# install arm-none-eabi (one package available but not working - apt install -y gcc-arm-none-eabi)
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 && tar xvf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C /opt
ENV PATH="/opt/gcc-arm-none-eabi-9-2020-q2-update/bin:${PATH}"

# install aarch64-none-elf (no packages available)
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz && tar xvf gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz -C /opt
ENV PATH="/opt/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin:${PATH}"

# install risc-v (RV64) compiler
COPY --from=gcc-riscv-build-stage /opt/gcc-riscv /opt/gcc-riscv
ENV PATH="/opt/gcc-riscv/bin:${PATH}"


# install qemu (arm and aarch64)
RUN apt install -y \
    qemu-utils  \
    qemu-efi-aarch64 \
    qemu-system-arm

# check qemu (arm)
RUN qemu-system-arm --version

# check qemu (aarch64)
RUN qemu-system-aarch64 --version

# install qemu (RV64)
COPY --from=qemu-build-stage /opt/qemu /opt/qemu
ENV PATH="/opt/qemu/bin:${PATH}"

RUN apt update && apt install -y \
    libpixman-1-0 \
    libglib2.0-0 \
    rsync

# check qemu (riscv)
RUN qemu-system-riscv64 --version

# check gcc version
RUN g++ --version

# get rtmlib from github
RUN git clone --branch v2.0.x --depth 1 https://github.com/anmaped/rtmlib.git /rtmlib
RUN cd /rtmlib && git submodule update --depth 1 --init --recursive
# or
COPY . /rtmlib
RUN cd /rtmlib/thirdparty \
    && rm -r /rtmlib/thirdparty/nuttx \
    && git clone https://github.com/apache/incubator-nuttx.git nuttx
RUN cd /rtmlib/thirdparty \
    && git clone https://github.com/apache/incubator-nuttx-apps.git nuttx-apps

# make examples
RUN cd /rtmlib/examples && make

#
# make and run rtmlib tests (x86)
#
RUN cd /rtmlib/tests && make clean && make ARCH=x86_64 BUILD_DIR=build/x86-linux && ./build/x86-linux/rtmlib_unittests

#
# make and run rtmlib tests with qemu (arm32)
#

# minimal test
RUN cd /rtmlib/tests/os/none/arm/minimal-example && ./make.sh
RUN qemu-system-arm -M mps2-an386 -cpu cortex-m4 -monitor none -nographic -serial stdio -semihosting -kernel /rtmlib/tests/os/none/arm/minimal-example/test.elf

# without OS
RUN cd /rtmlib/tests && make ARCH=arm OS=none CMSIS=/rtmlib/thirdparty/cmsis_5 BUILD_DIR=build/arm-none
RUN qemu-system-arm -M mps2-an386 -cpu cortex-m4 -monitor none -nographic -serial stdio -semihosting -kernel /rtmlib/tests/build/arm-none/rtmlib_unittests.elf

# with FreeRTOS

# freertos demo without rtmlib tests
RUN cd /rtmlib/examples/qemu-arm-mps2-freertos && make
#RUN qemu-system-arm \
#    -M mps2-an386 \
#    -cpu cortex-m4 \
#    -monitor none \
#    -nographic \
#    -serial stdio \
#    -kernel /rtmlib/examples/qemu-arm-mps2-freertos/output/qemu-mps2-freertos.elf

# freertos demo with rtmlib
RUN cp /rtmlib/tests/os/freertos/arm/*.c /rtmlib/examples/qemu-arm-mps2-freertos
RUN cd /rtmlib/tests && make ARCH=arm OS=freertos OS_PATH=/rtmlib/thirdparty/freertos-kernel CMSIS=/rtmlib/thirdparty/cmsis_5 BUILD_DIR=build/arm-freertos
RUN qemu-system-arm -M mps2-an386 -cpu cortex-m4 -monitor none -nographic -serial stdio -semihosting -kernel /rtmlib/tests/build/arm-freertos/rtmlib_unittests.elf

# with NuttX
# [TODO]

#
# make and run rtmlib tests with qemu (aarch64)
#

# [TODO]

# without OS
#RUN cd /rtmlib/tests && make clean && make ARCH=aarch64 OS=none CMSIS=/rtmlib/thirdparty/cmsis_5 BUILD_DIR=out


#
# make and run rtmlib tests with qemu (riscv64)
#


# without OS

RUN riscv64-unknown-elf-gcc -v
RUN cd /rtmlib/tests && make ARCH=riscv64 OS=none BUILD_DIR=build/riscv-none
RUN qemu-system-riscv64 -M virt -cpu rv64 -m 512M -smp 2 -monitor none -nographic -serial stdio -kernel /rtmlib/tests/build/riscv-none/rtmlib_unittests.elf

# with NuttX
RUN apt install -y \
    kconfig-frontends \
    genromfs \
    xxd

#RUN cd /rtmlib/thirdparty \
#    && git clone https://github.com/apache/incubator-nuttx.git nuttx \
#    && git clone https://github.com/apache/incubator-nuttx-apps.git nuttx-apps

RUN cd /rtmlib/thirdparty/nuttx \
    && ./tools/configure.sh -a ../nuttx-apps rv-virt:smp64 \
    && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"

# reference:
#   qemu-system-riscv64 -semihosting -M virt -cpu rv64 -smp 8 -bios none -kernel /rtmlib/thirdparty/nuttx/nuttx -nographic
#RUN qemu-system-riscv64 -M virt -cpu rv64 -m 512M -smp 8 -monitor none -nographic -serial stdio -bios none -kernel /rtmlib/thirdparty/nuttx/nuttx
