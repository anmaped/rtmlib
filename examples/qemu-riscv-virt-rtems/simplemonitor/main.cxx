/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2024 André Pedro
 *
 *  This file is part of rtmlib.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Hafiz Muhammad Waqas Qammar
 */

// Rtems headers
#include <pthread.h>
#include <rtems.h>
#include <rtems/posix/pthread.h>
#include <rtems/rtems/clock.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// include rtmlib headers
#include "task_compat.h"
#include "writer.h"

#include "Rtm_instrument_62be.h"
#include "Rtm_monitor_62be.h"

RTML_BUFFER0_SETUP();

// Declare  with C linkage.
extern "C" {
void *POSIX_Init(void *arg);
}

void monitor_test() {

  rtems_interval ticks_per_sec = 1 * rtems_clock_get_ticks_per_second();

  // Create a writer
  Writer_rtm__62be writer; // Creating an instance of Writer<int>

  // send A with 1s delay
  for (auto n = 0; n < 3; n++) {
    writer.push(Writer_rtm__62be::a);
    rtems_task_wake_after(1 * ticks_per_sec);
  }

  // send B with 1s delay
  for (auto n = 0; n < 3; n++) {
    writer.push(Writer_rtm__62be::b);
    rtems_task_wake_after(1 * ticks_per_sec);
  }

  return;
}

void *POSIX_Init(void *arg) {

  // Create trace object
  RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();

  rtm_mon0.enable();

  monitor_test();

  rtm_mon0.disable();

  assert(!rtm_mon0.join());

  auto v = rtm_mon0.getVeredict();

  if (strcmp(out_p(v), "true") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

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
