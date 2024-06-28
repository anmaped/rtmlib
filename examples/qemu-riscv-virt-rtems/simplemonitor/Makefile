PGM=${ARCH}/app.exe

# optional managers required
MANAGERS=all

# C++ source names
CXXSRCS = main.cxx
CXXOBJS_ = $(CXXSRCS:.cxx=.o) 
CXXOBJS = $(CXXOBJS_:%=${ARCH}/%)


RTEMS_MAKEFILE_PATH:=/opt/rtems/6/riscv-rtems6/rv64imafdc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CFLAGS_OPTIMIZE_V = -O0 -g -ffunction-sections -fdata-sections

DEFINES += -I../../../src
DEFINES += -D_GNU_SOURCE
DEFINES += -DRTMLIB_ENABLE_DEBUG_RMTLD3 -DRTMLIB_ENABLE_MAP_SORT

OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS) 

all:    ${ARCH} $(PGM)

$(PGM): $(OBJS)
	$(make-cxx-exe)
