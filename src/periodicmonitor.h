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

#ifndef RTML_PERIODICMONITOR_H
#define RTML_PERIODICMONITOR_H

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#ifdef __NUTTX__
#include <sched.h>
#endif

#include <errno.h>

#include "circularbuffer.h"
#include "task_compat.h"
#include "time_compat.h"

extern "C" void __cxa_pure_virtual() { while (1); }

/**
 * Periodic monitor.
 *
 * This class implements a periodic monitor that can execute with the system
 * tasks using a Posix-compliant interface. The monitoring task is seen like
 * any other task in the system.
 *
 * \warning
 * The function pointer for the monitor body must be assigned before activating
 * the periodic monitor, failure to do so will result in undefined behavior.
 *
 * @author André Pedro
 * @date
 */
class RTML_monitor {
private:
  /** Points to the monitor thread in the OS. */
  pthread_t thread;

  /** Enumeration type for monitor status. */
  enum mon_status { ACTIVATION=0, RUNNING, DELAY, ABORT, ABORTED, UNACTIVATE };

  /** Monitor state */
  struct Monitor_state {
    /** The schedulability policy according to the pthread lib. */
    const int sched_policy;

    /** The priority for the schedule policy, please see the pthread
     * documentation for more details. */
    const int priority;

    /**
     * Stack size
     */
    const size_t stack_size;

    /** Status of the monitor */
    mon_status status;

    /** Mutex for monitor */
    pthread_mutex_t fmtx;

    /** Conditional variable for monitor */
    pthread_cond_t cond;

    /** The Monitor's period. */
    useconds_t period;

    Monitor_state(const int sch, const int prio, const useconds_t p)
        : sched_policy(sch), priority(prio), stack_size(STACK_SIZE),
          status(UNACTIVATE), period(p){};

  } m_state;

  /**
   * A function pointer for the monitor body. The body content is executed in
   * the run loop and there is also synchronization and deadline miss detection
   * there.
   *
   * @param ptr pointer to the monitor to be executed.
   */
  static void *loop(void *);

protected:
  /**
   * The monitor execution control body.
   *
   * Users should overwrite this function with their monitoring function. It
   * will be called periodically as defined by the users scheduling policy.
   *
   */
  virtual void run() = 0;

public:
  /**
   * Instantiates a new monitor with a given period.
   *
   * The schedule policy will be SCHED_OTHER with priority 0.
   *
   * @param period the monitors period.
   */
  RTML_monitor(const useconds_t period);

  /**
   * Instantiates a new monitor with a given period, a schedule policy and a
   * priority.
   *
   * @param period the RTML_monitor period.
   * @param policy the posix schedule policy for this monitor.
   * @param priority the priority for this RTML_monitor.
   */
  RTML_monitor(const useconds_t period, unsigned int policy,
               unsigned int priority);

  /**
   * Enables the monitor
   */
  int enable();

  /**
   * Disables the monitor
   *
   * @param f the function to start the thread with.
   */
  int disable();

  /**
   * Checks whether this RTML_monitor is running.
   *
   * @returns true if the monitor is running.
   */
  bool isRunning() const;

  /**
   * Gets the period.
   * @return the monitor period.
   */
  const useconds_t &getPeriod() const;

  /** Sets new monitor period. */
  void setPeriod(const useconds_t &p);
};

RTML_monitor::RTML_monitor(const useconds_t period)
    : m_state(Monitor_state(SCHED_FIFO, 1, period)) {}

RTML_monitor::RTML_monitor(const useconds_t period,
                           unsigned int schedule_policy, unsigned int priority)
    : m_state(Monitor_state(schedule_policy, priority, period)) {}

void *RTML_monitor::loop(void *ptr) {
  RTML_monitor *monitor = (RTML_monitor *)ptr;
  struct timespec now = {}, next = {}, tmp = {};

  // Mutex and conditional variables for pthread_cond_timedwait
  pcheck_print(pthread_mutex_init(&monitor->m_state.fmtx, NULL), P_OK,
               return NULL;);
  pcheck_print(pthread_cond_init(&monitor->m_state.cond, NULL), P_OK,
               return NULL;);

  clock_gettime(CLOCK_REALTIME, &next);

  for (;;) {

    clock_gettime(CLOCK_REALTIME, &now);

    // convert useconds_t to struct timespec
    struct timespec p;

    useconds_t2timespec(&monitor->m_state.period, &p);

    DEBUGV3("period in us: %lu\n", monitor->m_state.period);
    DEBUGV3("period in (s, ns): %lu, %lu\n", p.tv_sec, p.tv_nsec);
    DEBUGV3("current time in (s, ns): %lu,%lu\n", now.tv_sec, now.tv_nsec);
    DEBUGV3("current last next time in (s, ns): %lu, %lu\n", next.tv_sec, next.tv_nsec);

    timespecadd(&next, &p, &next);

    DEBUGV3("next time in (s, ns): %lu, %lu\n", next.tv_sec, next.tv_nsec);

    if (timespeccmp(&now, &next, >)) {

      timespecsub(&next, &now, &tmp);
      DEBUGV_ERROR("RTML_monitor is missing their deadline for %lu s.%lu ns\n",
               tmp.tv_sec, tmp.tv_nsec);
    }

    pthread_mutex_lock(&monitor->m_state.fmtx);
    pcheck_print(pthread_cond_timedwait(&monitor->m_state.cond,
                                        &monitor->m_state.fmtx, &next),
                 ETIMEDOUT, break;);
    pthread_mutex_unlock(&monitor->m_state.fmtx);

    monitor->run();

    if (monitor->m_state.status == ABORT) {
      monitor->m_state.status = ABORTED;
      return NULL;
    }
  }

  return NULL;
}

int RTML_monitor::enable() {
  pthread_attr_t attribute = {};
  struct sched_param parameter;

  ::printf("RTML_monitor started!\n");

  if (!isRunning()) {

    pcheck(pthread_attr_init(&attribute));

    pcheck_attr(pthread_attr_setschedpolicy(&attribute, m_state.sched_policy),
                &attribute);

    pcheck_attr(pthread_attr_setstacksize(&attribute, m_state.stack_size),
                &attribute);
    DEBUGV("Stack:%lu\n", m_state.stack_size);

    parameter.sched_priority = m_state.priority;
    DEBUGV("Priority:%d\n", m_state.priority);

    pcheck_attr(pthread_attr_setschedparam(&attribute, &parameter), &attribute);

    pcheck_attr(pthread_create(&thread, &attribute, &loop, (void *)this),
                &attribute);

    pcheck(pthread_attr_destroy(&attribute));

    m_state.status = RUNNING;

  } else {
    // errno = EALREADY;
    return -1;
  }

  return P_OK;
}

int RTML_monitor::disable() {
  m_state.status = ABORT;
  return P_OK;
}

bool RTML_monitor::isRunning() const { return m_state.status == RUNNING; }

const useconds_t &RTML_monitor::getPeriod() const { return m_state.period; }

void RTML_monitor::setPeriod(const useconds_t &p) { m_state.period = p; }

#endif // RTML_PERIODICMONITOR_H
