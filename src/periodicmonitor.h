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

#include "circularbuffer.h"
#include "task_compat.h"
#include "time_compat.h"

extern "C" void __attribute__((weak)) __cxa_pure_virtual() {
  while (1)
    ;
}

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
template <char... name> class RTML_monitor {
private:
  /** Points to the monitor thread in the OS. */
  __task _task;

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
  /** Stores id */
  const char id[5] = {name...};

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

  int join();
};

template <char... name>
RTML_monitor<name...>::RTML_monitor(const useconds_t period)
    : _task(task(id, loop, DEFAULT_PRIORITY, DEFAULT_SCHED, period, this)) {}

template <char... name>
RTML_monitor<name...>::RTML_monitor(const useconds_t period,
                                    unsigned int schedule_policy,
                                    unsigned int priority)
    : _task(task(id, loop, priority, schedule_policy, period, this)) {}

template <char... name> void *RTML_monitor<name...>::loop(void *ptr) {
  RTML_monitor<name...> *monitor = (RTML_monitor *)ptr;
  monitor->run();

  return NULL;
}

template <char... name> int RTML_monitor<name...>::enable() {

  if (_task.st != ACTIVATE)
    _task.st = ACTIVATE;

  return P_OK;
}
template <char... name> int RTML_monitor<name...>::disable() {

  if (_task.st != ABORT)
    _task.st = ABORT;

  return P_OK;
}

template <char... name> bool RTML_monitor<name...>::isRunning() const {
  return _task.st == RUNNING;
}

template <char... name>
const useconds_t &RTML_monitor<name...>::getPeriod() const {
  return _task.period;
}

template <char... name>
void RTML_monitor<name...>::setPeriod(const useconds_t &p) {
  _task.period = p;
}

template <char... name> int RTML_monitor<name...>::join() {
  void *ret = NULL;
  if (pthread_join(_task.thread, &ret))
    return 1;

  return 0;
}

#endif // RTML_PERIODICMONITOR_H
