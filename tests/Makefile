
SRC_DIR := .
OBJ_DIR := output
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp' | sed -r "s/.+\/(.+)\..+/\1/")

$(shell mkdir -p $(OBJ_DIR))

# Debug Flags
#DEBUGFLAGS=-DDEBUG=3

# __x86__ (including __x86_64__) Flags
PREF := 
CFLAGS = -std=c11
CXXFLAGS := -std=c++11
CPPFLAGS := -mcx16 -I../src/ ${DEBUGFLAGS} # -mcx16 flag is required for gcc versions <7
LDFLAGS := -pthread -latomic

# arm (cortex-m4) Flags
ifeq ($(ARCH),arm)
	PREF := arm-none-eabi-
	CFLAGS = -std=c11
	CXXFLAGS := -std=c++11
	CPPFLAGS := -nostartfiles -mcpu=cortex-m4 -I../src/ ${DEBUGFLAGS}
	EXTENSION := .elf
	
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
		OBJ_FILES := $(OBJ_DIR)/startup.o $(OBJ_FILES) $(OBJ_DIR)/uart_pl011.o $(OBJ_DIR)/stub.o
		VPATH += $(SRC_DIR)/os/none/arm
	endif

	ifeq ($(OS),freertos)
$(eval $(shell make -f os/freertos/arm/Makefile print-VPATH FREERTOS_ROOT=$(OS_PATH)/FreeRTOS OUTPUT_DIR=$(OBJ_DIR)))
$(eval $(shell make -f os/freertos/arm/Makefile print-OBJS_OUTPUT FREERTOS_ROOT=$(OS_PATH)/FreeRTOS OUTPUT_DIR=$(OBJ_DIR)))
$(eval $(shell make -f os/freertos/arm/Makefile print-INCLUDE_DIRS FREERTOS_ROOT=$(OS_PATH)/FreeRTOS OUTPUT_DIR=$(OBJ_DIR)))

		WELCOME_MSG:=Welcome from ARMCM4 with FreeRTOS! \\\n
		CPPFLAGS += -ffreestanding \
			-D__FREERTOS__ \
			-I/rtmlib/freertos-posix/include \
			-I/rtmlib/freertos-posix/include/private \
			-I/rtmlib/freertos-posix/FreeRTOS-Plus-POSIX/include \
			-I/rtmlib/freertos-posix/FreeRTOS-Plus-POSIX/include/portable \
			-I/rtmlib/freertos-posix/FreeRTOS-Plus-POSIX/include/portable/st/stm32l475_discovery \
			-DconfigUSE_POSIX_ERRNO=1 \
			-DconfigUSE_APPLICATION_TASK_TAG=1 \
			$(INCLUDE_DIRS)
		
		SRC_DIR__:=/rtmlib/freertos-posix/FreeRTOS-Plus-POSIX/source
		SRC_FILES__ := $(wildcard $(SRC_DIR__)/*.c)
		OBJ_FILES__ := $(patsubst $(SRC_DIR__)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES__))

		CFLAGS +=  \
			-DINCLUDE_xTaskGetCurrentTaskHandle=1
				
		LDFLAGS := -T $(SRC_DIR)/os/freertos/arm/mps2_m3.ld -specs=nano.specs \
			-specs=nosys.specs -specs=rdimon.specs
		OBJ_FILES += $(OBJS_OUTPUT) $(OBJ_FILES__)
		VPATH += $(OS_PATH)/FreeRTOS/Demo/CORTEX_MPS2_QEMU_IAR_GCC/build/gcc/ $(SRC_DIR__)
		PREFIX_MAIN:=__task_
	endif

	#
	# enable mps2 uart (mps2-an386)
	#
	#UART_INIT_BODY+=\#include \"uart_pl011.h\" \n
	UART_INIT_BODY+=\#define UART0_ADDRESS 	( 0x40004000UL ) \n
	UART_INIT_BODY+=\#define UART0_CTRL		( * ( ( ( volatile uint32_t * )( UART0_ADDRESS + 8UL ) ) ) ) \n
	UART_INIT_BODY+=\#define UART0_BAUDDIV	( * ( ( ( volatile uint32_t * )( UART0_ADDRESS + 16UL ) ) ) ) \n
	UART_INIT_BODY+=\#define uart_init void _ () { UART0_BAUDDIV = 16;  UART0_CTRL = 1; }; _ \n
	UART_INIT+=uart_init(); \n
	#UART_INIT+=uart_putchar('f'); \n
	#UART_INIT+=uart_putchar('\\\n'); \n

endif

# aarch64 Flags
ifeq ($(ARCH),aarch64)
	PREF := 
	CFLAGS = -std=c11
	CXXFLAGS := -std=c++11
	CPPFLAGS := -I../src/ ${DEBUGFLAGS}
	LDFLAGS := 
	OBJ_FILES += $(SRC_DIR)/stub.o
endif

CC := $(PREF)gcc
CPP := $(PREF)g++


# generate .c file to join source code tests
MAIN_TEST_FILE:=$(patsubst %, extern int %(); \n, $(SOURCES))
MAIN_TEST_FILE+=\#include <stdio.h> \n
MAIN_TEST_FILE+=$(UART_INIT_BODY)
MAIN_TEST_FILE+=int $(PREFIX_MAIN)main(void) { \n
MAIN_TEST_FILE+=$(UART_INIT)
MAIN_TEST_FILE+=printf(\"$(WELCOME_MSG)\"); \n
MAIN_TEST_FILE+=$(patsubst %, %();\n, $(SOURCES))
MAIN_TEST_FILE+=while(1){  };
MAIN_TEST_FILE+=return 0; \n}

# create .c file to compile available source code tests
OUT := $(shell [ ! -f $(OBJ_DIR)/unittests.c ] && printf "$(MAIN_TEST_FILE)" > $(OBJ_DIR)/unittests.c)

$(OBJ_DIR)/rtmlib_unittests$(EXTENSION): $(OBJ_FILES) $(OBJ_DIR)/unittests.o
	${CPP} $(LDFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	${CPP} $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	$(PREF)as -c -mcpu=cortex-m4 -o $@ $<

clean:
	rm -rf output
