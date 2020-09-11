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

#ifndef _ATOMIC_COMPAT_H_
#define _ATOMIC_COMPAT_H_

// Architecture dependent macros for atomic operations

/*
 * There is a notable difference in the __HW__, __x86__ and __ARM_CM4__
 * implementations. ARM macros implement atomic operations using
 * Load-link/store-conditional instructions, x86 uses compare-exchange, and
 * __HW__ supports hardware synthesis.
 *
 * The implementation of rtmlib in ARM is quite restrictive. It ensures that any
 * inter-living is detected from any source, e.g. shared memory and interrupts
 * among cores. The x86 implementation is more relaxed and suffers from the ABA
 * problem, since the B thread can change the value to the one expected by the A
 * thread that already started the atomic operation. The implementation for
 * hardware synthesis does not consider any concurrency.
 */

#ifdef __HW__

typedef unsigned int uint32_t;

/*
 * disable lock-free atomic
 */
#define ATOMIC_PAGE()                                                          \
  {}
#define ATOMIC_PAGE_SWAP()                                                     \
  {}
#define ATOMIC(body)                                                           \
  { body }

#define _bottom() __bottom
#define _top() __top

#define ATOMIC_TRIPLE(b, t, ts)                                                \
  b = _bottom();                                                               \
  t = _top();                                                                  \
  ts = array[_bottom()].getTime();

/*
 *
 * arm and aarch64 atomic macros
 *
 */
#elif defined(ARM_CM4_FP)

#include <ARMCM4_FP.h>

#include <atomic>

/*
#define ATOMIC_begin(expression, dest)                                         \
  uint32_t __lst_;                                                             \
  __DMB();                                                                     \
  do {                                                                         \
    __lst_ = expression __LDREXW((uint32_t *)dest);

#define ATOMIC_end(dest)                                                       \
  }                                                                            \
  while (__STREXW(__lst_, (uint32_t *)dest))                                   \
    ;

#define DMB __DMB();
#define DSB __DSB();
#define ISB __ISB();
*/

#define FRAME_ADDRESS frame_address

#define OLD_FRAME_ADDRESS __old_value32

#define FRAME_ADDRESS_type std::atomic<uint32_t>

#define FRAME_ADDRESS_subtype uint32_t

#define ATOMIC_begin(dest)                                                     \
  bool fail = false;                                                           \
  uint32_t OLD_FRAME_ADDRESS = (uint32_t)std::atomic_load(&dest);              \
  do {                                                                         \
    if (fail) {                                                                \
      pthread_yield();                                                         \
    }

#define ATOMIC_end(new_value, dest)                                            \
  }                                                                            \
  while ((fail = !std::atomic_compare_exchange_strong(                         \
              &dest, &OLD_FRAME_ADDRESS, (uint32_t)new_value)))                \
    ;

#define ATOMIC_begin_VALUE64_NOEXCHANGE(dest)                                  \
  uint32_t OLD_FRAME_ADDRESS = (uint32_t)std::atomic_load(&dest);              \
  do {

#define ATOMIC_end_VALUE64_NOEXCHANGE(dest)                                    \
  }                                                                            \
  while (!(std::atomic_load(&dest) == OLD_FRAME_ADDRESS))                      \
    ;

#define NATIVE_POINTER_TYPE uint32_t
#define NATIVE_ATOMIC_POINTER uint32_t

/*
 *
 * x86 and x86_64 atomic macros
 *
 */
#elif defined(__x86__) || defined(__x86_64__)

#include <atomic>

#if defined(__x86__)
#define NATIVE_POINTER_TYPE uint32_t
#define NATIVE_ATOMIC_POINTER uint64_t
#else
#define NATIVE_POINTER_TYPE uint64_t
#define NATIVE_ATOMIC_POINTER __int128
#endif

/*
 * This union type is used to split a 64/128 bit wide integer into two equaly
 * divided variables; counter means the current index and pageref is a
 * reference that identifies who is doing the atomic operation. pageref should
 * be 64bit wide when using a 64bit architecture. This union type is not used in
 * arm architecture.
 */
union page_t {
  NATIVE_ATOMIC_POINTER wide_uniqueid;
  struct {
    NATIVE_POINTER_TYPE stateref;
    NATIVE_POINTER_TYPE counter;
  };
};

#define update(page)                                                           \
  {                                                                            \
    page.counter += 1;                                                         \
    page.stateref = (NATIVE_POINTER_TYPE)stateref;                             \
    page;                                                                      \
  }

struct __state {
  size_t top;
  size_t bottom;
};

typedef struct __state state_t;

#define _bottom()                                                              \
  ((state_t *)(((page_t)std::atomic_load(&page)).stateref))->bottom
#define _top() ((state_t *)(((page_t)std::atomic_load(&page)).stateref))->top

#define ATOMIC_TRIPLE(b, t, ts)                                                \
  state_t *s = ((state_t *)(((page_t)std::atomic_load(&page)).stateref));      \
  b = s->bottom;                                                               \
  t = s->top;                                                                  \
  ts = array[s->bottom].getTime();

#define ATOMIC_PAGE()                                                          \
  state_t state_global = {0};                                                  \
  std::atomic<page_t> page =                                                   \
      ATOMIC_VAR_INIT({(NATIVE_POINTER_TYPE)&state_global})

#define ATOMIC_PAGE_SWAP()                                                     \
  state_t state;                                                               \
  state_t *stateref = &state;

#define ATOMIC(body)                                                           \
  bool fail = false;                                                           \
  page_t current_page_content, new_page_content;                               \
  do {                                                                         \
    if (fail) {                                                                \
      /*pthread_yield();*/                                                     \
      fail = false;                                                            \
    }                                                                          \
                                                                               \
    current_page_content = (page_t)std::atomic_load(&buffer.page);             \
    new_page_content = current_page_content;                                   \
                                                                               \
    if (&state == (state_t *)current_page_content.stateref) {                  \
      /* current state is in use; use buffer state */                          \
      stateref = (state_t *)&buffer.state_global;                              \
    } else {                                                                   \
      /* use current state */                                                  \
      stateref = (state_t *)&state;                                            \
    }                                                                          \
                                                                               \
    /* copy state */                                                           \
    stateref->bottom = ((state_t *)current_page_content.stateref)->bottom;     \
    stateref->top = ((state_t *)current_page_content.stateref)->top;           \
                                                                               \
    body;                                                                      \
    DEBUGV("address:%p content:%p,%lu id:%p\n", &buffer.page,                  \
           (state_t *)current_page_content.stateref, current_page_content.counter,        \
           &state);                                                            \
  } while (                                                                    \
      fail = !(std::atomic_compare_exchange_strong(                            \
          &buffer.page, &current_page_content, (update(new_page_content)))));

#else

#warning "Atomic guarantees are not supported!"

#endif

#endif //_ATOMIC_COMPAT_H_
