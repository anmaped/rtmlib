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

#ifndef _DEBUG_COMPAT_H_
#define _DEBUG_COMPAT_H_

#ifndef DEBUG
#define DEBUG 0
#endif

#if defined(__arm__)
#if defined(__FREERTOS__)
#include <FreeRTOS.h>
#include <stdarg.h>
extern "C" int printf(const char *format, ...);
#else
#include <stdio.h>
#endif
#elif defined(__x86__) || defined(__x86_64__)
#include <stdio.h>
#define x86 1
#endif

#if defined(DEBUG) && DEBUG > 0 && defined(x86)

#define DEBUGV_ERROR(fmt, args...)                                             \
  fprintf(stderr, "ERROR: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,     \
          ##args)

#define DEBUGV(fmt, args...)                                                   \
  fprintf(stdout, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,     \
          ##args)

#define DEBUGV_APPEND(fmt, args...) fprintf(stdout, fmt, ##args)

#if defined(DEBUG) && DEBUG > 2 && defined(x86)

#define DEBUGV3(fmt, args...)                                                  \
  fprintf(stdout, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,     \
          ##args)

#define DEBUGV3_APPEND(fmt, args...) fprintf(stdout, fmt, ##args)

#else
#define DEBUGV3(...)

#define DEBUGV3_APPEND(...)

#endif

#elif defined(DEBUG) && DEBUG > 0 && defined(__arm__)

#define DEBUGV_ERROR(fmt, args...)                                             \
  printf("ERROR: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)

#define DEBUGV(fmt, args...)                                                   \
  printf("DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)

#define DEBUGV_APPEND(fmt, args...) printf(fmt, ##args)

#if defined(DEBUG) && DEBUG > 2 && defined(__arm__)

#define DEBUGV3(fmt, args...)                                                  \
  printf("DEBUG3: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)

#define DEBUGV3_APPEND(fmt, args...) printf(fmt, ##args)

#else

#define DEBUGV3(...)

#define DEBUGV3_APPEND(...)

#endif

#else

#define DEBUGV(...)
#define DEBUGV_APPEND(...)
#define DEBUGV3(...)
#define DEBUGV_ERROR(...)
#define DEBUGV3_APPEND(...)

#endif

// Helpers for debuging measures

#define START_MEASURE()                                                        \
  uint64_t start, stop;                                                        \
  volatile int cycle_count = 0;                                                \
  start = clockgettime();                                                      \
  // DEBUGV3("START_TIME: %lld\n", start);

#define COUNT_CYCLE() cycle_count++;

#define STOP_MEASURE()                                                         \
  stop = clockgettime();                                                       \
  DEBUGV("DURATION_TIME:%llu:%u\n", stop - start, cycle_count);

#endif //_DEBUG_COMPAT_H_
