// monitor-app

#include <nuttx/clock.h>
#include <nuttx/config.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern "C" int monitorapp_main(int argc, char *const argv[]);

extern "C" int __s_main(void);

static int daemon_task;

static int monitor_main_loop(int argc, char **argv) {

  __s_main();

  while (true) {
    sleep(1);
  };

  return 0;
}

int monitorapp_main(int argc, char *const argv[]) {

  printf("unittests app starting...\n");

  daemon_task =
      task_create("monitorapp_main", 241, 20048, monitor_main_loop, NULL);
 
  printf("unittests app started.\n");

  return 0;
}
