/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2022 André Pedro
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
 * There is a notable difference in the __HW__, __i386__ and __ARM_CM4__
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

/* Hardware settings */
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

/* Software */
#elif defined(__riscv) || defined(__arm__) || defined(__i386__) ||              \
    defined(__x86_64__)

/*
 *
 * RISC-V 64 atomic macros
 *
 */
#if defined(__riscv)

// #warning "Atomic guarantees on RISC-V are not supported yet!"
//  __riscv_atomic

#include <atomic>

#ifdef NO_THREADS
#define pthread_yield()
#elif defined(__FREERTOS__)
#include <FreeRTOS_POSIX/sched.h>
#define pthread_yield() (sched_yield())
#elif defined(__NUTTX__)
#include <pthread.h>
#endif

#define NATIVE_POINTER_TYPE uint64_t
#define NATIVE_ATOMIC_POINTER uint64_t

union page_t {
  NATIVE_ATOMIC_POINTER wide_uniqueid;
  struct {
    NATIVE_POINTER_TYPE stateref;
  };
};

#define update(page)                                                           \
  {                                                                            \
    page.stateref = (NATIVE_POINTER_TYPE)stateref;                             \
    page;                                                                      \
  }

#define DEBUG_FRAME()                                                          \
  DEBUGV("address:%p content:%p,%lu id:%p\n", &buffer.page,                    \
         (state_t *)current_page_content.stateref, &state);

/*
 *
 * ARM32 atomic macros
 *
 */
#elif defined(__arm__)

// #warning "Atomic guarantees on ARM32 are not supported yet!"

#include <atomic>

#ifdef NO_THREADS
#define pthread_yield()
#elif defined(__FREERTOS__)
#include <FreeRTOS_POSIX/sched.h>
#define pthread_yield() (sched_yield())
#endif

#define NATIVE_POINTER_TYPE uint32_t
#define NATIVE_ATOMIC_POINTER uint32_t

union page_t {
  NATIVE_ATOMIC_POINTER wide_uniqueid;
  struct {
    NATIVE_POINTER_TYPE stateref;
  };
};

#define update(page)                                                           \
  {                                                                            \
    page.stateref = (NATIVE_POINTER_TYPE)stateref;                             \
    page;                                                                      \
  }

#define DEBUG_FRAME()                                                          \
  DEBUGV("address:%p content:%p,%lu id:%p\n", &buffer.page,                    \
         (state_t *)current_page_content.stateref, &state);

/*
 *
 * x86 and x86_64 atomic macros
 *
 */
#elif defined(__i386__) || defined(__x86_64__)

#include <atomic>

#if defined(__i386__) && not defined(__x86_64__)
#define NATIVE_POINTER_TYPE uint32_t
#define NATIVE_ATOMIC_POINTER uint64_t
#elif defined(__x86_64__)
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

#define DEBUG_FRAME()                                                          \
  DEBUGV("address:%p content:%p,%lu id:%p\n", &buffer.page,                    \
         (state_t *)current_page_content.stateref,                             \
         current_page_content.counter, &state);

#endif

#define ATOMIC_TYPE()                                                          \
  struct __state {                                                             \
    size_t top;                                                                \
    size_t bottom;                                                             \
    /* variables that can help to confirm that the last value is ready */      \
    size_t pos;                                                                \
    event_t event;                                                             \
  };                                                                           \
                                                                               \
  typedef struct __state state_t

#define _bottom()                                                              \
  ((state_t *)(((page_t)std::atomic_load(&page)).stateref))->bottom
#define _top() ((state_t *)(((page_t)std::atomic_load(&page)).stateref))->top

#define ATOMIC_TRIPLE(b, t, ts)                                                \
  state_t *s = ((state_t *)(((page_t)std::atomic_load(&page)).stateref));      \
  b = s->bottom;                                                               \
  t = s->top;                                                                  \
  ts = array[s->bottom].getTime();

#define ATOMIC_PAGE()                                                          \
  state_t state_global = {                                                     \
      .top = 0, .bottom = 0, .pos = 0, .event = event_t()};                    \
  std::atomic<page_t> page =                                                   \
      ATOMIC_VAR_INIT({(NATIVE_POINTER_TYPE)&state_global})

#define ATOMIC_PAGE_SWAP()                                                     \
  typedef typename B::state_t state_t;                                         \
  state_t state;                                                               \
  state_t *stateref = &state;

#define ATOMIC_PUSH(body)                                                      \
  bool fail = false;                                                           \
  page_t current_page_content, new_page_content;                               \
  typename B::event_t evt;                                                     \
  do {                                                                         \
    if (fail) {                                                                \
      /* do a yield here */                                                    \
      pthread_yield();                                                         \
      fail = false;                                                            \
    }                                                                          \
                                                                               \
    current_page_content = (page_t)std::atomic_load(&buffer.page);             \
    new_page_content = current_page_content;                                   \
                                                                               \
  /* give a chance to complete the last push if it is not ready yet */         \
  complete:                                                                    \
    buffer.read(evt, ((state_t *)current_page_content.stateref)->pos);         \
    DEBUGV("%lu\n", ((state_t *)current_page_content.stateref)->pos);          \
    if (!(((state_t *)current_page_content.stateref)->event == evt)) {         \
      /* do a yield here */                                                    \
      pthread_yield();                                                         \
      goto complete;                                                           \
    }                                                                          \
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
    stateref->pos = stateref->top;                                             \
                                                                               \
    body;                                                                      \
                                                                               \
    /* copy remaining state */                                                 \
    stateref->event = event;                                                   \
                                                                               \
    DEBUG_FRAME();                                                             \
  } while (                                                                    \
      fail = !(std::atomic_compare_exchange_strong(                            \
          &buffer.page, &current_page_content, (update(new_page_content)))));

#else

#warning "Atomic guarantees are not supported!"

#endif

#endif //_ATOMIC_COMPAT_H_
