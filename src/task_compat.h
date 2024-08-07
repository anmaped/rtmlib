/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2020 André Pedro
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
 */

#ifndef _TASK_COMPAT_H_
#define _TASK_COMPAT_H_

#if defined(__NUTTX__) || defined(__NuttX__)
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#define STACK_SIZE 10000
#define DEFAULT_SCHED SCHED_FIFO
#define DEFAULT_PRIORITY 50
#elif defined(__FREERTOS__)
#include <FreeRTOS_POSIX.h>
#include <FreeRTOS_POSIX/errno.h>
#include <FreeRTOS_POSIX/pthread.h>
#include <FreeRTOS_POSIX/sys/types.h>
#define STACK_SIZE 10000
#define DEFAULT_SCHED SCHED_OTHER
#define DEFAULT_PRIORITY 4
#elif defined(__rtems__) 
#include <rtems.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/pthread.h>
#include <rtems/rtems/types.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sched.h>
#include <pthread.h>
#define STACK_SIZE 10000
#define DEFAULT_SCHED SCHED_OTHER
#define DEFAULT_PRIORITY 4
#elif defined(__linux__)
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#define STACK_SIZE 30000
#define DEFAULT_SCHED SCHED_FIFO
#define DEFAULT_PRIORITY 50
#else
#define STACK_SIZE 0
#define DEFAULT_SCHED 0
#define DEFAULT_PRIORITY 0
#warning "Tasks are not supported!"
#endif

#include "debug_compat.h"
#include "time_compat.h"

#define P_OK 0

#define pcheck(val)                                                            \
  if (val != P_OK) {                                                           \
    DEBUGV("Error: %d\n", val);                                                \
    return val;                                                                \
  }

#define pcheck_print(val, checkval, extra_action)                              \
  if (val != checkval) {                                                       \
    DEBUGV("Error: %d\n", val);                                                \
    extra_action                                                               \
  }

#define pcheck_attr(val, attr)                                                 \
  if (val != P_OK) {                                                           \
    DEBUGV("Destroying ERROR:%d\n", val);                                      \
    pthread_attr_destroy(attr);                                                \
    return val;                                                                \
  }

enum status { ACTIVATE, UNACTIVATE, RUNNING, DELAY, ABORT, ABORTED };

#ifdef __HW__

struct task {

  char const *tid;

  useconds_t period;

  int sched_policy;

  int priority;

  status st;

  void *(*run)(void *);

  void *run_payload;

  task(char const *id, void *(*loop)(void *), const int prio,
       const int sch_policy, const useconds_t p, void *payload = NULL)
      : tid(id), period(p), sched_policy(sch_policy), priority(prio), run(loop),
        run_payload(payload) {}
};

#endif

#ifndef __HW__

struct task {

  pthread_t thread;

  pthread_mutex_t fmtx;

  pthread_cond_t cond;

  char const *tid;

  useconds_t period;

  const int sched_policy;

  const int priority;

  status st;

  void *(*run)(void *);

  void *run_payload;

  int create_task(void *(*loop)(void *), const int pri, const int s_policy,
                  int stack_size = STACK_SIZE) {
    pthread_attr_t attribute = {0};
    struct sched_param parameter;

    DEBUGV("#Task(%s) creation sizeof(pthread_mutex_t)=%d "
           "sizeof(pthread_cond_t)=%d ",
           tid, sizeof(pthread_mutex_t), sizeof(pthread_cond_t));

    pcheck(pthread_attr_init(&attribute));

    pcheck_attr(pthread_attr_setschedpolicy(&attribute, sched_policy),
                &attribute);

    pcheck_attr(pthread_attr_setstacksize(&attribute, stack_size), &attribute);
    DEBUGV_APPEND("stack=%u ", stack_size);

    DEBUGV_APPEND("priority=%u", priority);
    parameter.sched_priority = priority;

    pcheck_attr(pthread_attr_setschedparam(&attribute, &parameter), &attribute);

    pcheck_attr(pthread_create(&thread, &attribute, loop, this), &attribute);

    pcheck(pthread_attr_destroy(&attribute));

    DEBUGV_APPEND("\n");

    st = UNACTIVATE;

    return 0;
  }

  task(char const *id, void *(*loop)(void *), const int prio,
       const int sch_policy, const useconds_t p, void *payload = NULL)
      : tid(id), period(p), sched_policy(sch_policy), priority(prio), run(loop),
        run_payload(payload) {
    create_task(
        [](void *tsk) -> void * {
          struct task *ttask = (struct task *)tsk;
          struct timespec now = {0}, next = {0}, tmp = {0};

          DEBUGV("#Task(%s) is waiting ...\n", ttask->tid);

          // Mutex and conditional variables for pthread_cond_timedwait
          pcheck_print(pthread_mutex_init(&ttask->fmtx, NULL), P_OK,
                       return NULL;);
          pcheck_print(pthread_cond_init(&ttask->cond, NULL), P_OK,
                       return NULL;);

          // trap to block task start
          for (;;) {
            if (ttask->st == ACTIVATE) {
              ttask->st = RUNNING;
              break;
            }
            nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
          }

          DEBUGV("#Task(%s) is running ...\n", ttask->tid);

          // initialize the current gettime after task initalization
          clock_gettime(CLOCK_REALTIME, &next);

          for (;;) {

            DEBUGV3("#Task(%s) job: ", ttask->tid);

            clock_gettime(CLOCK_REALTIME, &now);

            DEBUGV3_APPEND("sizeof(struct timespec)=0x%x job_time%lus.%luns ",
                           sizeof(struct timespec), L(now.tv_sec), now.tv_nsec);

            DEBUGV3_APPEND("period=%luus ", ttask->period);

            // convert useconds_t to struct timespec
            useconds_t2timespec(&ttask->period, &tmp);

            DEBUGV3_APPEND("period=%lus.%luns ", L(tmp.tv_sec), tmp.tv_nsec);
            DEBUGV3_APPEND("old_next=%lus.%luns ", L(next.tv_sec),
                           next.tv_nsec);

            // calculate new next task wake up
            timespecadd(&next, &tmp, &next);

            DEBUGV3_APPEND("new_next=%lus.%luns ", L(next.tv_sec),
                           next.tv_nsec);
            DEBUGV3_APPEND("\n");

            // check if task is missing its deadlines
            if (timespeccmp(&now, &next, >)) {

              timespecsub(&next, &now, &tmp);
              DEBUGV_ERROR("#T(%s): missing its deadline in %lus.%luns\n",
                           ttask->tid, L(tmp.tv_sec), tmp.tv_nsec);
            }

            pthread_mutex_lock(&ttask->fmtx);
            pcheck_print(
                pthread_cond_timedwait(&ttask->cond, &ttask->fmtx, &next),
                ETIMEDOUT, break;);
            pthread_mutex_unlock(&ttask->fmtx);

            if (ttask->st == ABORT) {
              ttask->st = ABORTED;
              return NULL;
            }

            ttask->run(ttask->run_payload);
          }

          return NULL;
        },
        prio, sch_policy);
  }
};

#endif

typedef struct task __task;

#endif //_TASK_COMPAT_H_
