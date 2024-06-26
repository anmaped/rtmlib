PGM=${ARCH}/monitorapp.exe

# C++ source names
CXXSRCS = main.cxx
CXXOBJS_ = $(CXXSRCS:.cxx=.o) 
CXXOBJS = $(CXXOBJS_:%=${ARCH}/%)

EXTRA_OBJS =../../../../../build/riscv-rtems/rtmlib_unittests.o


RTEMS_MAKEFILE_PATH:=/opt/rtems/6/riscv-rtems6/rv64imafdc


include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS) $(EXTRA_OBJS)

DEFINES += -I../../../../src
DEFINES += -DDEBUG=0 -D_GNU_SOURCE 

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-cxx-exe)

# qemu-system-riscv64 -semihosting -M virt -cpu rv64  -nographic -bios o-optimize/monitorapp.exe
