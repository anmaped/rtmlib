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

#ifndef _TIME_COMPAT_H_
#define _TIME_COMPAT_H_

#include "atomic_compat.h"

#define L(x) ((timespan) (x))

#ifdef __HW__

typedef long long timeabs;
typedef long timespan;

#define clockgettime() 0

/*
 *
 * RISC-V time macros
 *
 */
#elif (__riscv)

#include <time.h>

typedef uint64_t timeabs;
typedef uint32_t timespan;

#if defined(__NUTTX__)
#include <nuttx/clock.h>

#define clockgettime()                                                         \
  ({                                                                           \
    struct timespec __n;                                                       \
    clock_gettime(CLOCK_REALTIME, &__n);                                       \
    uint64_t result = __n.tv_sec;                                              \
    (result * 1000000000) + (__n.tv_nsec);                                     \
  })

#else

#define clockgettime() 0 // [TODO]

#endif

/*
 *
 * ARM32 time macros
 *
 */
#elif (__arm__)

/*
 * to use systick include CMSIS
 */
#if defined(CORTEX_M7)
#include <ARMCM7.h>
#elif defined(CORTEX_M4)
#include <ARMCM4.h>
#elif defined(CORTEX_M3)
#include <ARMCM3.h>
#elif defined(CORTEX_M0)
#include <ARMCM0.h>
#elif defined(CORTEX_M0PLUS)
#include <ARMCM0plus.h>
#else
#error "Processor not specified or unsupported."
#endif

/*
 * We can interpret timestamps as uint8_t, uint16_t, uint24_t, uint32_t, and
 * uint64_t. Note that timestamps are measured in nanoseconds.
 *
 *  - uint8_t only contains 255 ticks and is small; It means that we have to
 *  execute the monitors as fast as 255 ticks, inducing a huge overhead;
 *
 *  - uint16_t can be enough for faster monitors (not so good for low-end
 * Cortex-M processors);
 *
 *  - uint24_t is enough if the monitor has a period no bigger than 100hz;
 *
 *  - uint32_t is enough if the monitor has a period no bigger than 1hz; and
 *
 *  - for all the other cases uint64_t or bigger is required.
 */
typedef uint64_t timeabs;
typedef uint32_t timespan;

#define NOP __NOP()

#if defined(__NUTTX__)
#include <nuttx/clock.h>

// get cpu_clock
// assuming 168mhz 1000000000/(168000000/167999)
// (1000000000/(clock/SysTick->LOAD))/SysTick->LOAD currently ~= 5.5(5)ns

#define clockgettime()                                                         \
  ({                                                                           \
    timespan ns = SysTick->VAL;                                                \
    uint64_t ms = g_system_timer;                                              \
    (ms * 1000000) + ((1000000. / SysTick->LOAD) * (SysTick->LOAD - ns));      \
  })

#elif defined(__FREERTOS__)
#include <FreeRTOS_POSIX/time.h>

#define clockgettime()                                                         \
  ({                                                                           \
    struct timespec __n;                                                       \
    clock_gettime(CLOCK_REALTIME, &__n);                                       \
    uint64_t result = __n.tv_sec;                                              \
    (result * 1000000000) + (__n.tv_nsec);                                     \
  })

#else
#define SYSTEM_TIMER() 0 // [TODO]

#define clockgettime()                                                         \
  ({                                                                           \
    timespan ns = SysTick->VAL;                                                \
    uint64_t ms = SYSTEM_TIMER();                                              \
    (ms * 1000000) + ((1000000. / SysTick->LOAD) * (SysTick->LOAD - ns));      \
  })

#endif

/*
 *
 * x86 and x86_64 time macros
 *
 */
#elif defined(__x86__) || defined(__x86_64__)

#include <pthread.h>
#include <time.h>

typedef long long timeabs;
typedef long timespan;

#define clockgettime()                                                         \
  ({                                                                           \
    struct timespec __n;                                                       \
    clock_gettime(CLOCK_REALTIME, &__n);                                       \
    uint64_t result = __n.tv_sec;                                              \
    (result * 1000000000) + (__n.tv_nsec);                                     \
  })

#else

#error                                                                         \
    "This monitoring library only supports NuttX ARM Cortex-M4, x86 and x86_64 architectures!"

#endif

typedef timeabs timespanw;

/* Operations on timespecs. */
#define timespecclear(tsp) (tsp)->tv_sec = (tsp)->tv_nsec = 0

#define timespecisset(tsp) ((tsp)->tv_sec || (tsp)->tv_nsec)

#define timespeccmp(tsp, usp, cmp)                                             \
  (((tsp)->tv_sec == (usp)->tv_sec) ? ((tsp)->tv_nsec cmp(usp)->tv_nsec)       \
                                    : ((tsp)->tv_sec cmp(usp)->tv_sec))

#define timespecadd(tsp, usp, vsp)                                             \
  do {                                                                         \
    (vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;                             \
    (vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;                          \
    if ((vsp)->tv_nsec >= 1000000000L) {                                       \
      (vsp)->tv_sec++;                                                         \
      (vsp)->tv_nsec -= 1000000000L;                                           \
    }                                                                          \
  } while (0)

#define timespecsub(tsp, usp, vsp)                                             \
  do {                                                                         \
    (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;                             \
    (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;                          \
    if ((vsp)->tv_nsec < 0) {                                                  \
      (vsp)->tv_sec--;                                                         \
      (vsp)->tv_nsec += 1000000000L;                                           \
    }                                                                          \
  } while (0)

#define useconds_t2timespec(tsp, vsp)                                          \
  do {                                                                         \
    (vsp)->tv_sec = *tsp / 1000000L;                                           \
    (vsp)->tv_nsec = (*tsp % 1000000L) * 1000;                                 \
  } while (0)

#endif //_TIME_COMPAT_H_
