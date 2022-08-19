#

all:
	echo "NuttX App Demo with RISC-V."

	# install dependencies
	sudo apt install -y kconfig-frontends genromfs xxd

	# copy demo from ../../examples/qemu-riscv-virt-nuttx/demo

	echo "help" >> ../../thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/src/etc/init.d/rcS

	# compile nuttx with the demo
	cd ../../thirdparty && mkdir -p nuttx-apps
	cd ../../thirdparty/nuttx && ./tools/configure.sh -a ../nuttx-apps rv-virt:smp64
	cd ../../thirdparty/nuttx && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"
