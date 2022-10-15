#

all:
	echo "NuttX App Demo with RISC-V."

	# install dependencies
ifneq ($(shell id -u), 0)
	@echo "You are not root, run this target as root ..."
	sudo apt install -y kconfig-frontends genromfs xxd
else
	apt update
	apt install -y kconfig-frontends genromfs xxd
endif

	# copy demo from ../../examples/qemu-riscv-virt-nuttx/demo

	echo "help" >> ../../thirdparty/nuttx/boards/risc-v/qemu-rv/rv-virt/src/etc/init.d/rcS

	pwd
	#exit 1
	# compile nuttx with the demo
	#cd ../../thirdparty && mkdir -p nuttx-apps
		
	cd ../../thirdparty/nuttx && cp -r ../../examples/qemu-riscv-virt-nuttx/demo ../nuttx-apps/
	
	cd ../../thirdparty/nuttx && ./tools/configure.sh -a ../nuttx-apps rv-virt:smp64
	cd ../../thirdparty/nuttx && make ARCHCPUFLAGS="-O0 -mcmodel=medany -march=rv64gc -mabi=lp64d"
