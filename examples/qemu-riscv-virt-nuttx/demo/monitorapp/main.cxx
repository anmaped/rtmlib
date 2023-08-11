// monitor-app

// NuttX headers
#include <nuttx/clock.h>
#include <nuttx/config.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// include rtmlib headers
#include "task_compat.h"
#include "writer.h"

// include rmtld3synth monitor header
#include "Rtm_monitor_0ee2.h"

extern "C" int monitorapp_main(int argc, char *const argv[]);

static int daemon_task;

RTML_BUFFER0_SETUP();

static int monitor_main_loop(int argc, char **argv) {

  // enable rmtld3synth monitor(s)
  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  while (true) {
    sleep(1);
  };

  return OK;
}

int monitorapp_main(int argc, char *const argv[]) {

  printf("Monitor App starting...\n");

  // base address
  printf("Buffer base address is %lx\n", (long unsigned int)&__buffer_rtm_monitor_0ee2);

  daemon_task =
      task_create("monitorapp_main", 241, 2048, monitor_main_loop, NULL);

  printf("Monitor App started.\n");

  return OK;
}
