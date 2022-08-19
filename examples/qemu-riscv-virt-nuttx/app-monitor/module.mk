#
# rtmlib test module for NuttX RTOS
#
# For gcc 4.7.2 you have to use the libatomic explicitly; It's included by default since 4.9.x version.
#

# editable settings...
MONITOR_SOURCE_FILES = mon1.cpp
MONITOR_FOLDER = $(SKETCHBOOK)/modules/rtmlib/nuttx/mon1
MONITOR_TEST_CASES_FOLDER = $(MONITOR_FOLDER)/tests

# do not edit these part
TO_COMPILE=$(addprefix $(MONITOR_FOLDER)/, $(MONITOR_SOURCE_FILES))

MODULE_COMMAND = rtmlib_app
SRCS = main.cpp ../RTML_monitor.cpp $(TO_COMPILE)
MODULE_STACKSIZE = 4096
EXTRACXXFLAGS = \
  -Wframe-larger-than=1200 \
  -DCONFIG_WCHAR_BUILTIN \
  -I$(SKETCHBOOK)/modules/rtmlib \
  -I$(SKETCHBOOK)/modules/rtmlib/arch/arm/include \
  -I$(MONITOR_FOLDER) \
  -I$(MONITOR_TEST_CASES_FOLDER) \
  -DARM_CM4_FP \
  -D__NUTTX__ \
  -std=c++0x \
  --verbose \
  -w
