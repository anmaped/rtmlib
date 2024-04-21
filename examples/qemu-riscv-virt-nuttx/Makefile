# NuttX Example Makefile

all:
	echo "NuttX App Demo with RISC-V."

	# suggest installing dependencies
	@echo "You may need to install dependencies, e.g., on debian 'apt install kconfig-frontends genromfs xxd git'."

	# download nuttx and compile it with the demo apps

	git clone --branch nuttx-10.3.0 --depth 1 https://github.com/apache/nuttx.git
	git clone --branch nuttx-10.3.0 --depth 1 https://github.com/apache/nuttx-apps.git

	# copy examples/qemu-riscv-virt-nuttx/demo directory
	cp -r demo nuttx-apps/

	ls nuttx-apps/ -l

	echo "help" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/src/etc/init.d/rcS

	echo "CONFIG_NSH_ARCHROMFS=y" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig \

	echo "CONFIG_NSH_ROMFSETC=y" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig \
	echo "CONFIG_FS_ROMFS=y" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig
	echo "CONFIG_INIT_STACKSIZE=3072" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig
	echo "CONFIG_SYSTEM_NSH_STACKSIZE=3072" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/configs/nsh64/defconfig

	cd nuttx && ./tools/configure.sh -a ../nuttx-apps/ rv-virt:nsh64
	cd nuttx && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"
