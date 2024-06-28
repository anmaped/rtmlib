FROM debian:11 as qemu-build-stage
LABEL Description="qemu RV64"

RUN apt update && apt install -y \
    build-essential \
    git \
    python3 \
    pkg-config \
    ninja-build \
    libglib2.0-dev \
    libpixman-1-dev && \
    rm -rf /var/lib/apt/lists/*

RUN cd /tmp && \
    git clone --depth 1 --branch v6.1.0 https://gitlab.com/qemu-project/qemu.git qemu-riscv-src && \
    cd qemu-riscv-src && \
    ./configure --prefix=/opt/qemu --target-list=riscv64-softmmu && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/riscv-gnu-toolchain    


FROM debian:11 as gcc-riscv-build-stage
LABEL Description="gcc RV64"

RUN apt update && apt install -y \
    build-essential \
    git \
    curl \
    gawk \
    bison \
    flex \
    texinfo \
    zlib1g-dev \
    libexpat-dev && \
    rm -rf /var/lib/apt/lists/*

RUN git clone -b 2021.04.23 --depth 1 https://github.com/riscv/riscv-gnu-toolchain /tmp/riscv-gnu-toolchain && \
    cd /tmp/riscv-gnu-toolchain  && \
    git submodule init && \
    git submodule update --depth 1

RUN mkdir -p /opt/gcc-riscv && \
    cd /tmp/riscv-gnu-toolchain && \
    ./configure --prefix=/opt/gcc-riscv --enable-gdb --with-cmodel=medany --with-libatomic && \
    make -j $(nproc) && \
    rm -rf /tmp/riscv-gnu-toolchain    


FROM debian:11 as rtems-build-stage
LABEL Description="rtems6 RV64 toolchain"
ENV TERM linux

RUN apt-get update && apt-get install -y \
    apt-utils \ 
    build-essential \ 
    git \
    python3.9 \
    python3.9-dev \
    python-is-python3 \
    bison \
    flex \
    pkg-config \
    ninja-build \
    libglib2.0-dev \
    libpixman-1-dev \
    unzip \
    wget \
    xxd && \
    rm -rf /var/lib/apt/lists/*

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

# Build the RTEMS toolchain
RUN  cd /usr/src && \
    git clone https://gitlab.rtems.org/rtems/tools/rtems-source-builder.git rsb && \
    cd /usr/src/rsb/rtems && \
    echo "%define gmp_url https://ftp.gnu.org/gnu/gmp" >> config/6/rtems-riscv.bset && \
    ../source-builder/sb-set-builder --prefix=/opt/rtems/6 6/rtems-riscv && \
    cd /usr/src && \
    rm -rf rsb

ENV PATH="/opt/rtems/6/bin:${PATH}"
RUN riscv-rtems6-gcc --version

# Build the RTEMS Kernel / bsp
RUN  cd /usr/src && \
    git clone https://gitlab.rtems.org/rtems/rtos/rtems.git && \
    cd /usr/src/rtems && \
    echo "[riscv/rv64imafdc]" >> rv64-config.ini && \
    echo "RTEMS_POSIX_API = True" >> rv64-config.ini && \
    ./waf configure --target=riscv-rtems6 --prefix=/opt/rtems/6 --rtems-config=rv64-config.ini && \
    ./waf && \
    ./waf install


FROM debian:11
LABEL Description="rtmlib2 tests"

RUN apt update && apt install -y \
    build-essential \
    wget \
    git \
    qemu-utils  \
    qemu-efi-aarch64 \
    qemu-system-arm \
    libpixman-1-0 \
    libglib2.0-0 \
    rsync \
    kconfig-frontends \
    genromfs \
    xxd && \
    rm -rf /var/lib/apt/lists/*

# check qemu (arm)
RUN qemu-system-arm --version

# check qemu (aarch64)
RUN qemu-system-aarch64 --version

# install arm-none-eabi (one package available but not working - apt install -y gcc-arm-none-eabi)
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 && tar xvf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C /opt
ENV PATH="/opt/gcc-arm-none-eabi-9-2020-q2-update/bin:${PATH}"

# install aarch64-none-elf (no packages available)
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-a/9.2-2019.12/binrel/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz && tar xvf gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf.tar.xz -C /opt
ENV PATH="/opt/gcc-arm-9.2-2019.12-x86_64-aarch64-none-elf/bin:${PATH}"

# install risc-v (RV64) compiler
COPY --from=gcc-riscv-build-stage /opt/gcc-riscv /opt/gcc-riscv
ENV PATH="/opt/gcc-riscv/bin:${PATH}"

# install risc-v rtems (RV64) compiler
COPY --from=rtems-build-stage /opt/rtems /opt/rtems
ENV PATH="/opt/rtems/6/bin:${PATH}"

# check rtems version
RUN riscv-rtems6-gcc --version

# install qemu (RV64)
COPY --from=qemu-build-stage /opt/qemu /opt/qemu
ENV PATH="/opt/qemu/bin:${PATH}"

# check qemu (riscv)
RUN qemu-system-riscv64 --version

# check gcc version
RUN g++ --version

# get rtmlib from github
RUN git clone --branch v2.1.x --depth 1 https://github.com/anmaped/rtmlib.git /rtmlib
RUN cd /rtmlib && git submodule update --depth 1 --init --recursive
# or
COPY examples /rtmlib/examples
COPY src /rtmlib/src
COPY tests /rtmlib/tests

#
# make examples
#
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

# without OS
# [TODO] RUN cd /rtmlib/tests && make clean && make ARCH=aarch64 OS=none CMSIS=/rtmlib/thirdparty/cmsis_5 BUILD_DIR=out


#
# make and run rtmlib tests with qemu (riscv64)
#


# without OS

RUN riscv64-unknown-elf-gcc -v
RUN cd /rtmlib/tests && make ARCH=riscv64 OS=none BUILD_DIR=build/riscv-none
RUN qemu-system-riscv64 -M virt -cpu rv64 -m 512M -smp 2 -monitor none -nographic -serial stdio -kernel /rtmlib/tests/build/riscv-none/rtmlib_unittests.elf

# with NuttX

# configure and build config.h
RUN cd /rtmlib/thirdparty/nuttx \
    && ./tools/configure.sh -a ../nuttx-apps rv-virt:nsh64 \
    && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"

# build unittests (depends on NuttX config.h)
RUN cd /rtmlib/tests && make ARCH=riscv64 OS=nuttx BUILD_DIR=build/riscv-nuttx

# debug symbols and list object files
RUN ls /rtmlib/tests/build/riscv-nuttx -l
RUN nm /rtmlib/tests/build/riscv-nuttx/rtmlib_unittests.o

# add support for nsh startup scripts
RUN echo "CONFIG_NSH_ARCHROMFS=y" >> /rtmlib/thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig \
    && echo "CONFIG_NSH_ROMFSETC=y" >> /rtmlib/thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig \
    && echo "CONFIG_FS_ROMFS=y" >> /rtmlib/thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig

# include object /rtmlib/tests/build/riscv-nuttx/rtmlib_unittests.o and build NuttX
RUN cd /rtmlib/thirdparty/nuttx \
    && make distclean && make apps_distclean \
    && cp -r /rtmlib/tests/os/nuttx/riscv/unittests ../nuttx-apps/ \
    && echo "monitorapp" >> /rtmlib/thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/src/etc/init.d/rcS \
    && ./tools/configure.sh -a ../nuttx-apps rv-virt:nsh64 \
    && ls ../nuttx-apps/ -l \
    && make \
       ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d" \
       EXTRA_OBJS="/rtmlib/tests/build/riscv-nuttx/rtmlib_unittests.o"

RUN qemu-system-riscv64 -semihosting -M virt -cpu rv64 -bios none -kernel /rtmlib/thirdparty/nuttx/nuttx -nographic

# with RTEMS

# build unittests for RTEMS
RUN cd /rtmlib/tests && make ARCH=riscv64 OS=rtems BUILD_DIR=build/riscv-rtems

# include build object and build RTEMS
RUN cd /rtmlib/tests/os/rtems/riscv/unittests/monitorapp \
    && make

RUN qemu-system-riscv64 -semihosting -M virt -cpu rv64 -nographic -bios /rtmlib/tests/os/rtems/riscv/unittests/monitorapp/o-optimize/monitorapp.exe
