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
 
#ifndef _DEBUG_COMPAT_H_
#define _DEBUG_COMPAT_H_

#ifndef DEBUG
    #define DEBUG 1
#endif

#if defined (__x86__) || defined (__x86_64__)
    #define x86 1
#else
    #define x86 0
#endif

#if defined (DEBUG) && DEBUG > 0 && x86



#define DEBUGV_ERROR(fmt, args...) fprintf(stderr, "ERROR: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)

#define DEBUGV(fmt, args...) fprintf(stdout, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)


#if defined (DEBUG) && DEBUG > 2 && x86

    #define DEBUGV3(fmt, args...) fprintf(stdout, "DEBUG: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##args)

    #define DEBUGV3_APPEND(fmt, args...) fprintf(stdout, fmt, \
    ##args)

#else
    #define DEBUGV3(...)

    #define DEBUGV3_APPEND(...)

#endif



#elif defined (DEBUG) && DEBUG > 0 && defined (ARM_CM4_FP)


#define DEBUGV_ERROR(fmt, args...) ::printf("ERROR: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)

#define DEBUGV(fmt, args...) ::printf("DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)


#if defined (DEBUG) && DEBUG > 2 && defined (ARM_CM4_FP)

    #define DEBUGV3(fmt, args...) ::printf("DEBUG3: %s:%d:%s(): " fmt, \
        __FILE__, __LINE__, __func__, ##args)

    #define DEBUGV3_APPEND(fmt, args...) ::printf(fmt, ##args)

#else

    #define DEBUGV3(...)

    #define DEBUGV3_APPEND(...)

#endif

#else

// dummy macros
#define DEBUGV(...)
#define DEBUGV3(...)
#define DEBUGV_ERROR(...)
#define DEBUGV3_APPEND(...)

#endif


// Helpers for debuging measures

#define START_MEASURE() \
	uint64_t start, stop; \
    volatile int cycle_count=0; \
    start = clockgettime(); \
    //DEBUGV3("START_TIME: %lld\n", start);

#define COUNT_CYCLE() \
    cycle_count++;

#define STOP_MEASURE() \
    stop = clockgettime(); \
    DEBUGV("DURATION_TIME:%llu:%u\n", stop-start, cycle_count);

#endif //_DEBUG_COMPAT_H_
