# NuttX Example Makefile

all:
	echo "NuttX App Demo with RISC-V."

	# install dependencies
ifneq ($(shell id -u), 0)
	@echo "You need super user to install dependencies."
	@echo "Running sudo ..."
	sudo apt install -y kconfig-frontends genromfs xxd git
else
	apt update
	apt install -y kconfig-frontends genromfs xxd git
endif

	# download nuttx and compile it with the demo apps

	git clone --branch nuttx-10.3.0 --depth 1 https://github.com/apache/nuttx.git
	git clone --branch nuttx-10.3.0 --depth 1 https://github.com/apache/nuttx-apps.git

	# copy examples/qemu-riscv-virt-nuttx/demo directory
	cp -r demo nuttx-apps/

	ls nuttx-apps/ -l

	echo "help" >> ./nuttx/boards/risc-v/qemu-rv/rv-virt/src/etc/init.d/rcS

	cd nuttx && ./tools/configure.sh -a ../nuttx-apps/ rv-virt:nsh64
	cd nuttx && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"
