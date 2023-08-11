// app

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

extern "C" int app_main(int argc, char *const argv[]);

// [TODO: construct the header file instrumentation - Rtm_instrument_0ee2.h ]
#include "Rtm_instrument_0ee2.h"

typedef unsigned int proposition;
#define RTML_BUFFER0_SIZE 100
#define RTML_BUFFER0_TYPE Event< proposition >

extern "C" RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE> __buffer_rtm_monitor_0ee2;

static int daemon_task;

static int app_main_loop(int argc, char **argv) {

  // do something

  // send A - 2s -> B - 3s -> C
  // send B - 3s -> C
  // send C - 1s -> B

  RTML_writer<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>> writer =
      RTML_writer<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>(__buffer_rtm_monitor_0ee2);

  RTML_BUFFER0_TYPE node0 = RTML_BUFFER0_TYPE();

  writer.push(node0);

  while (true) {
    sleep(1);
  };

  return 0;
}

int app_main(int argc, char *const argv[]) {

  printf("App starting...\n");

  daemon_task =
      task_create("app_main", 241, 2048, app_main_loop, NULL);

  printf("App started.\n");

  return 0;
}
