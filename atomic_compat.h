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

#ifndef _ATOMIC_COMPAT_H_
#define _ATOMIC_COMPAT_H_

// Architecture dependent macros for atomic operations

/* 
 * There is a notable difference between __x86__ and __ARM_CM4__
 * implementations. ARM macros implement the atomic operations using the
 * Load-link/store-conditional instructions and x86 uses the compare-exchange.
 *
 * Then, the ARM implementation is too restrictive. It ensures that any
 * inter-living is detected from any source e.g. interrupts and shared memory
 * among cores. The x86 implementation is too relaxed and suffers from the ABA
 * problem since the thread B can change the value to the expected one by the
 * thread A that begun the atomic operation.
 *
 */

#ifdef ARM_CM4_FP


	#include <ARMCM4_FP.h>

 	#include <atomic>

	#define ATOMIC_begin(expression, dest) \
 		uint32_t __lst_; \
		__DMB(); \
	    do { \
	        __lst_ = expression __LDREXW((uint32_t *) dest);

	#define ATOMIC_end(dest) \
	       } while ( __STREXW(__lst_, (uint32_t *) dest) );

	#define DMB __DMB();
	#define DSB __DSB();
	#define ISB __ISB();

	#define FRAME_ADDRESS \
	    frame_address

	#define OLD_FRAME_ADDRESS \
	    __old_value32

	#define FRAME_ADDRESS_type \
	    std::atomic<uint32_t>

	#define FRAME_ADDRESS_subtype uint32_t
	

	#define ATOMIC_begin_VALUE64(dest) \
		bool fail = false; \
        uint32_t OLD_FRAME_ADDRESS = (uint32_t) std::atomic_load(&dest); \
        do { \
        	if(fail) { pthread_yield(); }

	#define ATOMIC_end_VALUE64(new_value, dest) \
	} while((fail = !std::atomic_compare_exchange_strong(&dest, &OLD_FRAME_ADDRESS, (uint32_t)new_value)));


	#define ATOMIC_begin_VALUE64_NOEXCHANGE(dest) \
        uint32_t OLD_FRAME_ADDRESS = (uint32_t) std::atomic_load(&dest); \
        do { 

	#define ATOMIC_end_VALUE64_NOEXCHANGE(dest) \
	} while( !( std::atomic_load(&dest) == OLD_FRAME_ADDRESS ) );

	#define NATIVE_POINTER_TYPE uint32_t
	#define NATIVE_ATOMIC_POINTER uint32_t

#elif defined (__x86__) || defined (__x86_64__)

	#include <atomic>

	#define DMB 

	#define FRAME_ADDRESS \
	    __counter

	#define OLD_FRAME_ADDRESS \
	    __old_value64

	#define FRAME_ADDRESS_type \
	    std::atomic<uint64_t>

	#define FRAME_ADDRESS_subtype uint64_t


	#define ATOMIC_begin_VALUE64(dest) \
		bool fail = false; \
        uint64_t OLD_FRAME_ADDRESS = (uint64_t) std::atomic_load(&dest); \
        do { \
        	if(fail) { sched_yield(); }


	#define ATOMIC_end_VALUE64(new_value, dest) \
	} while((fail = !std::atomic_compare_exchange_strong(&dest, &OLD_FRAME_ADDRESS, (uint64_t)new_value)));

	#define ATOMIC_begin_VALUE64_NOEXCHANGE(dest) \
        uint64_t OLD_FRAME_ADDRESS = (uint64_t) std::atomic_load(&dest); \
        do { 

	#define ATOMIC_end_VALUE64_NOEXCHANGE(dest) \
	} while( !( std::atomic_load(&dest) == OLD_FRAME_ADDRESS ) );

    #if defined (__x86_64__)
	  #define NATIVE_POINTER_TYPE uint64_t
	  typedef unsigned __int128 uint128_t;
	  #define NATIVE_ATOMIC_POINTER uint128_t
	#else
	  #define USE_DOUBLE_CAS
	  #define NATIVE_POINTER_TYPE uint32_t
	  #define NATIVE_ATOMIC_POINTER uint64_t
	#endif

#else

	#warning "Atomic guarantees are not supported!"

	#define ATOMIC_begin()
	#define ATOMIC_end()

#endif

#endif //_ATOMIC_COMPAT_H_
