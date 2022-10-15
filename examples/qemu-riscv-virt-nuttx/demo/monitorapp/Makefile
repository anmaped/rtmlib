include $(APPDIR)/Make.defs

# monitorapp info

PROGNAME = monitorapp
PRIORITY = 100
STACKSIZE = $(CONFIG_DEFAULT_TASK_STACKSIZE)
MODULE = $(CONFIG_DEMO_MONITOR_APP)

# monitorapp application

MAINSRC = main.cxx

CXXFLAGS += \
  -I$(APPDIR)/../../src \
  -I$(APPDIR)/demo/monitorapp/mon

include $(APPDIR)/Application.mk