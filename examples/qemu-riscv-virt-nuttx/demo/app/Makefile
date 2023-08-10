include $(APPDIR)/Make.defs

# app info

PROGNAME = app
PRIORITY = 100
STACKSIZE = $(CONFIG_DEFAULT_TASK_STACKSIZE)
MODULE = $(CONFIG_DEMO_MONITOR_APP)

# app application

MAINSRC = main.cxx

CXXFLAGS += \
  -I$(APPDIR)/../../../src \
  -I$(APPDIR)/demo/app

include $(APPDIR)/Application.mk
