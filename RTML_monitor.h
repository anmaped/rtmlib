/*
 *  rtmlib is a Real-Time Monitoring Library.
 *  This library was initially developed in CISTER Research Centre as a proof
 *  of concept by the current developer and, since then it has been freely
 *  maintained and improved by the original developer.
 *
 *    Copyright (C) 2018 André Pedro
 *
 *  This file is part of rtmlib.
 *
 *  rtmlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtmlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtmlib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTML_MONITOR_H
#define RTML_MONITOR_H

#include <time.h>
#include <pthread.h>
#include <stdio.h>

#ifdef __NUTTX__
#include <sched.h>
#endif

#include <errno.h>

#include "time_compat.h"
#include "RTML_buffer.h"

#include "task_compat.h"

/**
 * Represents a periodic RTML_monitor.
 *
 * This class represents a periodic monitor, capable of monitoring several
 * Ring_buffers. The monitor creates a periodic thread that executes the
 * code by the function run.
 *
 * \warning
 * The monitor must always exist in memory after enabling it, failure to do so
 * will result in an undefined behavior.
 * *
 * @author André Pedro
 * @date
 */
class RTML_monitor {
private:

    /** Points to the monitor's thread. */
    pthread_t thread;

    /** Type indentifying the monitor status. */
    enum mon_status {ACTIVATION, RUNNING, DELAY, ABORT, ABORTED, UNACTIVATE};

    /** Monitor's state  */
    struct Monitor_state
    {
        /** The schedule policy for the current RTML_monitor as defined in pthread. */
        const int sched_policy;

        /** The priority for the schedule policy, please see the pthread documentation for more information. */
        const int priority;

        /** Status of the monitor */
        mon_status status;

        /** Mutex for monitor */
        pthread_mutex_t fmtx;

        /** Conditional variable for monitor */
        pthread_cond_t cond;

        /** The Monitors period. */
        useconds_t period;

        Monitor_state(const int sch, const int prio, const useconds_t p) :
            sched_policy(sch),
            priority(prio),
            status(UNACTIVATE),
            period(p) {};

    } m_state;

    /**
     * Receives a monitor's pointer and executes their content by the run
     * function. The run loops checks for syncronization and deadline misses.
     *
     * @param ptr pointer to the monitor being ran.
     */
    static void* loop(void*);

protected:

    /**
     * The monitor execution body.
     *
     * Users should overwrite this function with their monitoring function. It will be called periodically as defined by
     * the users scheduling policy.
     *
     */
    virtual void run() = 0;

public:

    /**
     * Instantiates a new monitor with a period of timespec.
     *
     * The schedule policy will be SCHED_OTHER with priority 0.
     *
     * @param period the monitors period.
     */
    RTML_monitor(const useconds_t period);

    /**
     * Instantiates a new monitor with a certain period, a schedule policy and one priority.
     *
     * @param period the RTML_monitor period.
     * @param policy the posix schedule policy for this monitor.
     * @param priority the priority for this RTML_monitor.
     */
    RTML_monitor(const useconds_t period, unsigned int policy, unsigned int priority);

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
     * Returns the Monitors period.
     * @return the monitor period.
     */
    const useconds_t & getPeriod() const;

    /** Sets new monitor period. */
    void setPeriod(const useconds_t & p);
};

#endif //RTML_MONITOR_H
