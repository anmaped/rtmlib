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
#include <rtems.h>
#include <rtems/rtems/clock.h>
#include <rtems/posix/pthread.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// include rtmlib headers
#include "task_compat.h"
#include "writer.h"

#include "Rtm_monitor_ba2b.h"
#include "Rtm_instrument_ba2b.h"
#include "time_compat.h"


RTML_BUFFER0_SETUP();

// Declare  with C linkage.
extern "C" {
    void *POSIX_Init(void *arg);
}

void *POSIX_Init(void *arg)
{
    rtems_interval ticks_per_sec = 1 * rtems_clock_get_ticks_per_second();
    
    // Create trace object
    RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS();
    
    // Create a writer
    Writer_rtm__ba2b writer; // Creating an instance of Writer<int>

    // send A with 2s delay
    timespan t;
    for ( t = 0; t < 3; t++) {
        writer.push(Writer_rtm__ba2b::a, t);
        __trace_rtm_monitor_ba2b_0.synchronize();
        rtems_task_wake_after(2 * ticks_per_sec);
    }

    // send B with 2s delay
    for ( t = 0; t < 3; t++) {
        writer.push(Writer_rtm__ba2b::b, t);
        __trace_rtm_monitor_ba2b_0.synchronize();
        rtems_task_wake_after(2 * ticks_per_sec);
    }
    
    pthread_exit( 0 );
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
