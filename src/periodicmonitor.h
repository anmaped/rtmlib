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

extern "C" void __cxa_pure_virtual() {
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
class RTML_monitor {
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
    : _task(task("loop", loop, 50, SCHED_OTHER, period, this)) {}

RTML_monitor::RTML_monitor(const useconds_t period,
                           unsigned int schedule_policy, unsigned int priority)
    : _task(task("loop", loop, priority, schedule_policy, period, this)) {}

void *RTML_monitor::loop(void *ptr) {
  RTML_monitor *monitor = (RTML_monitor *)ptr;
  monitor->run();

  return NULL;
}

int RTML_monitor::enable() {

  ::printf("RTML_monitor started!\n");

  // [TODO: check status]

  return P_OK;
}

int RTML_monitor::disable() {

  // [TODO: check status]
  _task.st = ABORT;

  return P_OK;
}

bool RTML_monitor::isRunning() const { return !_task.running; }

const useconds_t &RTML_monitor::getPeriod() const { return _task.period; }

void RTML_monitor::setPeriod(const useconds_t &p) { _task.period = p; }

#endif // RTML_PERIODICMONITOR_H
