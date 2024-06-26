// monitor-app


#include <rtems/rtems/clock.h>
#include <rtems/posix/pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bsp.h>
#include <rtems/bspIo.h>

#include <unistd.h>

extern "C" {
  int __s_main(void);
  void *POSIX_Init(void *arg);
};


void *POSIX_Init(void *arg)
{
  printf("unittests app starting...\n");
  __s_main();
  printf("unittests app finished.\n");

  return NULL;
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE


#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE (64 * 1024)


#define CONFIGURE_INIT
#include <rtems/confdefs.h>
