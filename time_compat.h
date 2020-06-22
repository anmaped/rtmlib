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

#ifndef _TIME_COMPAT_H_
#define _TIME_COMPAT_H_

#include "atomic_compat.h"

#ifdef __HW__

typedef long long timeabs;
typedef long timespan;


#define clockgettime() 0

#elif (__NUTTX__)
#include <nuttx/clock.h>

/* 
 * We may interpret timestamps as uint8_t, uint16_t, uint24_t, uint32_t, and
 * uint64_t.
 *
 *  - uint8_t only contains 255 ticks and is small; It means that we have to
 *  execute the monitors as fast as 255 ticks, inducing a huge overhead.
 *
 *  - uint16_t can be enough for faster monitors (the Cortex-M is excluded)
 *
 *  - uint24_t is enough if the monitor has a period no bigger than 100hz
 *
 *  - uint32_t is enough if the monitor has a period no bigger than 1hz
 *
 *  - for all the other cases uint64_t is required
 *
 * In this note, the timestamps are measured in nanoseconds.
 */
typedef uint64_t timeabs;
typedef uint32_t timespan;


#define NOP __NOP()

// we need to adjust the systick when is triggering the IRQ
#define clockgettime()  ({ \
    ISB \
    timespan ns = SysTick->VAL; \
    uint64_t ms = g_system_timer; \
    ISB \
    (ms * 1000000) +  \
    ((1000000. / SysTick->LOAD) * \
    (SysTick->LOAD - ns));})

// get cpu_clock
// assuming 168mhz 1000000000/(168000000/167999)
// (1000000000/(clock/SysTick->LOAD))/SysTick->LOAD currently ~= 5.5(5)ns

#elif defined (__x86__) || defined (__x86_64__)

#include <time.h>
#include <pthread.h>

typedef long long timeabs;
typedef long timespan;


#define clockgettime() ({ \
    struct timespec __n; \
    clock_gettime(CLOCK_REALTIME, &__n); \
    uint64_t result = __n.tv_sec; \
    (result * 1000000000) + (__n.tv_nsec);})

#else

    #error "This monitoring library only supports NuttX ARM Cortex-M4 and x86 architecture!"

#endif

typedef timeabs timespanw;

/* Operations on timespecs. */
#define timespecclear(tsp)      (tsp)->tv_sec = (tsp)->tv_nsec = 0

#define timespecisset(tsp)      ((tsp)->tv_sec || (tsp)->tv_nsec)

#define timespeccmp(tsp, usp, cmp)                  \
    (((tsp)->tv_sec == (usp)->tv_sec) ?             \
        ((tsp)->tv_nsec cmp (usp)->tv_nsec) :           \
        ((tsp)->tv_sec cmp (usp)->tv_sec))

#define timespecadd(tsp, usp, vsp)                  \
    do {                                \
        (vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;      \
        (vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;   \
        if ((vsp)->tv_nsec >= 1000000000L) {            \
            (vsp)->tv_sec++;                \
            (vsp)->tv_nsec -= 1000000000L;          \
        }                           \
    } while (0)

#define timespecsub(tsp, usp, vsp)                  \
    do {                                \
        (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;      \
        (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;   \
        if ((vsp)->tv_nsec < 0) {               \
            (vsp)->tv_sec--;                \
            (vsp)->tv_nsec += 1000000000L;          \
        }                           \
    } while (0)

#define useconds_t2timespec(tsp, vsp)                 \
    do {                                            \
    	(vsp)->tv_sec = *tsp / 1000000L;            \
    	(vsp)->tv_nsec = (*tsp % 1000000L) * 1000;  \
    } while (0)

#endif //_TIME_COMPAT_H_
