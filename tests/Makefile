#
#  rtmlib is a Real-Time Monitoring Library.
#
#    Copyright (C) 2018-2022 André Pedro
#
#  This file is part of rtmlib.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

SRC_DIR := .
BUILD_DIR := output
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp' | sed -r "s/.+\/(.+)\..+/\1/")

$(shell mkdir -p $(BUILD_DIR))

# MAIN_TEST_FILE
PREAMBLE:=
MAIN_PREAMBLE:=
MAIN_EPILOGUE:=
EPILOGUE:=

# Debug Flags
#DEBUGFLAGS= -DRTMLIB_ENABLE_DEBUG_RMTLD3 -DRTMLIB_ENABLE_DEBUGV_RMTLD3 #-DRTMLIB_DEBUG=3 -g

ifneq (,$(filter x$(ARCH),x xx86 xx86_64))

# __x86__ (including __x86_64__) Flags
PREF := 
CFLAGS = -std=c11
CXXFLAGS := -std=c++11 -g
CPPFLAGS := -mcx16 -I../src/ ${DEBUGFLAGS} # -mcx16 flag is required for gcc versions <7
LDFLAGS := -pthread -latomic

endif

# arm (cortex-m4) Flags
ifeq ($(ARCH),arm)
	PREF := arm-none-eabi-
	CFLAGS = -std=c11
	CXXFLAGS := -std=c++11
	CPPFLAGS := -nostartfiles -mcpu=cortex-m4 -I../src/ ${DEBUGFLAGS}
	EXTENSION := .elf
	ASFLAGS := -mcpu=cortex-m4
	
	ifneq ($(CMSIS),)
		CPPFLAGS += -I$(CMSIS)/CMSIS/Core/Include/ \
			-I$(CMSIS)/CMSIS/Driver/Include/       \
			-I$(CMSIS)/Device/ARM/ARMCM4/Include/  \
			-DCORTEX_M4
	else
$(error Missing CMSIS!)
	endif

	ifeq ($(OS),none)
		WELCOME_MSG:=Welcome from ARMCM4 without OS! \\\n
		CPPFLAGS += -DNO_THREADS
		LDFLAGS := -T $(SRC_DIR)/os/none/arm/mps2_m3.ld
		OBJ_FILES := $(BUILD_DIR)/startup.o $(OBJ_FILES) $(BUILD_DIR)/uart_pl011.o $(BUILD_DIR)/stub.o
		VPATH += $(SRC_DIR)/os/none/arm
	endif

	ifeq ($(OS),freertos)
$(eval $(shell make \
	-f $(SRC_DIR)/../examples/qemu-arm-mps2-freertos/Makefile \
	print-VPATH \
	KERNEL_DIR=$(SRC_DIR)/../thirdparty/freertos-kernel \
	DEMO_PROJECT=$(SRC_DIR)/../examples/qemu-arm-mps2-freertos \
	OUTPUT_DIR=$(BUILD_DIR)))

$(eval $(shell make \
	-f $(SRC_DIR)/../examples/qemu-arm-mps2-freertos/Makefile \
	print-OBJS_OUTPUT \
	KERNEL_DIR=$(SRC_DIR)/../thirdparty/freertos-kernel \
	DEMO_PROJECT=$(SRC_DIR)/../examples/qemu-arm-mps2-freertos \
	OUTPUT_DIR=$(BUILD_DIR)))

$(eval $(shell make \
	-f $(SRC_DIR)/../examples/qemu-arm-mps2-freertos/Makefile \
	print-INCLUDE_DIRS \
	KERNEL_DIR=$(SRC_DIR)/../thirdparty/freertos-kernel \
	DEMO_PROJECT=$(SRC_DIR)/../examples/qemu-arm-mps2-freertos \
	OUTPUT_DIR=$(BUILD_DIR)))

		WELCOME_MSG:=Welcome from ARMCM4 with FreeRTOS! \\\n
		CPPFLAGS += -ffreestanding \
			-D__FREERTOS__ \
			-I$(OS_PATH)/../freertos-posix/include \
			-I$(OS_PATH)/../freertos-posix/include/private \
			-I$(OS_PATH)/../freertos-posix/FreeRTOS-Plus-POSIX/include \
			-I$(OS_PATH)/../freertos-posix/FreeRTOS-Plus-POSIX/include/portable \
			-I$(OS_PATH)/../freertos-posix/FreeRTOS-Plus-POSIX/include/portable/st/stm32l475_discovery \
			-DconfigUSE_POSIX_ERRNO=1 \
			-DconfigUSE_APPLICATION_TASK_TAG=1 \
			$(INCLUDE_DIRS)
				
		SRC_DIR__:=$(OS_PATH)/../freertos-posix/FreeRTOS-Plus-POSIX/source
		SRC_FILES__ := $(wildcard $(SRC_DIR__)/*.c)
		OBJ_FILES__ := $(patsubst $(SRC_DIR__)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES__))

		CFLAGS += 
		LDFLAGS := -T $(SRC_DIR)/os/freertos/arm/mps2_m3.ld -specs=nano.specs \
			-specs=nosys.specs -specs=rdimon.specs
		OBJ_FILES += $(OBJS_OUTPUT) $(OBJ_FILES__)
		VPATH += $(SRC_DIR)/../examples/qemu-arm-mps2-freertos $(SRC_DIR__)
		PREFIX_MAIN:=__task_
	endif

	#
	# enable mps2 uart (mps2-an386)
	#
	#PREAMBLE+=\#include \"uart_pl011.h\" \n
	PREAMBLE+=\#define UART0_ADDRESS 	( 0x40004000UL ) \n
	PREAMBLE+=\#define UART0_CTRL		( * ( ( ( volatile uint32_t * )( UART0_ADDRESS + 8UL ) ) ) ) \n
	PREAMBLE+=\#define UART0_BAUDDIV	( * ( ( ( volatile uint32_t * )( UART0_ADDRESS + 16UL ) ) ) ) \n
	PREAMBLE+=\#define uart_init void _ () { UART0_BAUDDIV = 16;  UART0_CTRL = 1; }; _ \n
	
	MAIN_PREAMBLE+=uart_init(); \n
	MAIN_PREAMBLE+=printf(\"$(WELCOME_MSG)\"); \n
	#MAIN_PREAMBLE+=uart_putchar('f'); \n
	#MAIN_PREAMBLE+=uart_putchar('\\\n'); \n

	# close qemu
	MAIN_EPILOGUE+=register int r0 __asm__(\"r0\"); \n
	MAIN_EPILOGUE+=r0 = 0x18; \n
	MAIN_EPILOGUE+=register int r1 __asm__(\"r1\"); \n
	MAIN_EPILOGUE+=r1 = 0x20026; \n
	MAIN_EPILOGUE+=__asm__ volatile(\"bkpt \#0xAB\"); \n

	EPILOGUE:=

endif

# aarch64 Flags
ifeq ($(ARCH),aarch64)
	PREF := 
	CFLAGS = -std=c11
	CXXFLAGS := -std=c++11
	CPPFLAGS := -I../src/ ${DEBUGFLAGS}
	LDFLAGS := 
	OBJ_FILES +=  $(BUILD_DIR)/uart_pl011.o $(SRC_DIR)/stub.o
endif

# riscv64 Flags
ifeq ($(ARCH),riscv64)
	PREF := riscv64-unknown-elf-
	CFLAGS = -std=c11
	CXXFLAGS := -std=c++11
	CPPFLAGS := -ffreestanding -nostartfiles -O0 -mcmodel=medany -march=rv64gc -mabi=lp64d -I../src/ ${DEBUGFLAGS}
	EXTENSION := .elf

	ifeq ($(OS),none)
		OBJ_FILES += $(SRC_DIR)/os/none/riscv/startup.o $(SRC_DIR)/os/none/riscv/uart.o $(SRC_DIR)/os/none/riscv/target.o
		LDFLAGS := -T $(SRC_DIR)/os/none/riscv/link.ld
		ASFLAGS := 
		CPPFLAGS += -DNO_THREADS -I$(SRC_DIR)/os/none/riscv

		PREAMBLE+=\#include \"uart.h\" \n
		MAIN_PREAMBLE+=uart_init(); \n
		#MAIN_PREAMBLE+=print_c('f'); \n
		#MAIN_PREAMBLE+=print_c('\\\n'); \n
		MAIN_EPILOGUE+=register int r0 __asm__(\"x5\"); \n
		MAIN_EPILOGUE+=r0 = 0x5555; \n
		MAIN_EPILOGUE+=register int r1 __asm__(\"x6\"); \n
		MAIN_EPILOGUE+=r1 = 0x100000; \n
		MAIN_EPILOGUE+=__asm__ volatile(\"sw x5, 0(x6)\"); \n

		VPATH += $(SRC_DIR)/os/none/riscv
	endif

	ifeq ($(OS),nuttx)
		OBJ_FILES += 
		LDFLAGS := -r
		ASFLAGS := 
		CPPFLAGS += -nostdlib -fno-exceptions -fno-threadsafe-statics -D__NUTTX__ -I../thirdparty/nuttx/include

		PREAMBLE +=
		MAIN_PREAMBLE +=
		MAIN_EPILOGUE+=register int r0 __asm__(\"x5\"); \n
		MAIN_EPILOGUE+=r0 = 0x5555; \n
		MAIN_EPILOGUE+=register int r1 __asm__(\"x6\"); \n
		MAIN_EPILOGUE+=r1 = 0x100000; \n
		MAIN_EPILOGUE+=__asm__ volatile(\"sw x5, 0(x6)\"); \n
		
		VPATH +=
		PREFIX_MAIN:= __s_
		EXTENSION := .o
	endif

endif

CC := $(PREF)gcc
CPP := $(PREF)g++
AS := $(PREF)as


# generate .c file to join source code tests
MAIN_TEST_FILE:=\#include <stdio.h> \n
MAIN_TEST_FILE+=$(patsubst %,extern int %(); \n, $(SOURCES))
MAIN_TEST_FILE+=$(PREAMBLE)
MAIN_TEST_FILE+=int $(PREFIX_MAIN)main(void) { \n
MAIN_TEST_FILE+=$(MAIN_PREAMBLE)
MAIN_TEST_FILE+=$(patsubst %,%();\n, $(SOURCES))
MAIN_TEST_FILE+=$(MAIN_EPILOGUE)
MAIN_TEST_FILE+=return 0; \n}
MAIN_TEST_FILE+=$(EPILOGUE)

# create .c file to compile available source code tests
OUT := $(shell [ ! -f $(BUILD_DIR)/unittests.c ] && printf "$(MAIN_TEST_FILE)" > $(BUILD_DIR)/unittests.c)

$(BUILD_DIR)/rtmlib_unittests$(EXTENSION): $(OBJ_FILES) $(BUILD_DIR)/unittests.o
	${CPP} $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	${CPP} $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	$(AS) -c $(ASFLAGS) -o $@ $<

clean:
	rm -rf output
