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

#ifndef _RTML_CIRCULARBUFFER_H_
#define _RTML_CIRCULARBUFFER_H_

#include "debug_compat.h"
#include "event.h"

#include <cstdint>

#ifndef __HW__
#include <cstring>
#define MEMCPY(x, y, z) std::memcpy(x, y, z)
#else
#warning "There is not memcpy implementation!"
#endif

/**
 * RTML_buffer implements a circular buffer. This buffer is the support
 * for RTML_reader and RTML_writer classes. For instance, The monitor uses
 * an instance of RTML_reader and SUO uses RTML_writer.
 *
 * @see Event
 * @see RTML_reader
 * @see RTML_monitor
 *
 * @author André Pedro
 * @date
 */
template <typename T, size_t N> class RTML_buffer {
private:
  /**
   * The T buffer where data is kept. The size is defined via template
   * parameter N which avoids dynamic memory usage.
   *
   * @see T
   */
  T array[N + 2];

  /**
   * The top of the circular buffer
   */
  size_t __top;

  /**
   * The bottom of the circular buffer
   */
  size_t __bottom;

  /**
   * The writer count variable that indicates the number of writers that have
   * been attached.
   */
  uint8_t writer; // max 255

  /**
   * Increment top of the buffer
   */
  size_t &increment_top() {
    size_t &t = _top();
    if (++t >= size)
      t = 0;
    return t;
  };

  /**
   * Decrement top of the buffer
   */
  size_t &decrement_top() {
    size_t &t = _top();
    if (t == 0)
      t = size - 1;
    else
      --t;
    return t;
  };

  /**
   * Increment bottom of the buffer
   */
  size_t &increment_bottom() {
    size_t &b = _bottom();
    if (++b >= size)
      b = 0;
    return b;
  };

public:
  const size_t size = N + 1;
  const size_t size_util = N;

  typedef T event_t;

  typedef enum { OK = 0, EMPTY, BUFFER_OVERFLOW, OUT_OF_BOUND, UNSAFE } error_t;

  ATOMIC_TYPE();
  ATOMIC_PAGE();

  /**
   * Instantiates a new RTML_buffer.
   */
  RTML_buffer();

  /**
   * Push a node
   */
  error_t push(const event_t &);

  /**
   * Pull a node in FIFO order
   */
  error_t pull(event_t &);

  /**
   * Pop event
   */
  error_t pop(event_t &);

  /**
   * Get the node at index without changing the state
   */
  error_t read(event_t &, size_t) const;

  /**
   * Set a node at the index without changing the state
   */
  error_t write(event_t &, size_t);

  /**
   * Get the state of the buffer without timestamps
   */
  error_t state(size_t &, size_t &) const;

  /**
   * Get the state of the buffer with timestamps
   */
  error_t state(size_t &, size_t &, timespanw &, timespanw &) const;

  /**
   * Get the length of the buffer
   */
  size_t length() const;

  /**
   * Print the state and the buffer content into the stdout
   */
  void debug() const;

  /**
   * Copy
   */
  RTML_buffer &operator=(const RTML_buffer &);

  /**
   * Increment writer counter
   */
  void increment_writer() {
    writer++;
  }
};

template <typename T, size_t N>
RTML_buffer<T, N>::RTML_buffer() : __top(0), __bottom(0), writer(0) {}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::push(const event_t &node) {

  size_t &t = _top();
  size_t &b = _bottom();
  array[t] = node;

  increment_top();

  bool p = t == b;

  if (p) {
    // discard one element; the buffer is now moving forward and
    // replacing the last elements (it may originate a gap)
    increment_bottom();
  }

  DEBUGV3("push-> %d (%d,%d) r:%d\n", length(), b, t, p);

  return (p) ? BUFFER_OVERFLOW : (writer > 1 ? UNSAFE : OK);
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::pull(event_t &event) {
  bool c = length() > 0;
  if (c) {
    event = array[_bottom()];
    increment_bottom();
  }

  DEBUGV3("pull-> %d (%d,%d) r:%d e:%d\n", length(), _bottom(), _top(), c,
          event.getTime());

  return c ? (writer > 1 ? UNSAFE : OK) : EMPTY;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::pop(event_t &event) {
  bool c = length() > 0;
  if (c) {
    size_t &t = decrement_top();
    event = array[t];
  }

  DEBUGV3("pop-> %d (%d,%d) r:%d e:%d\n", length(), _bottom(), _top(), c,
          event.getTime());

  return c ? (writer > 1 ? UNSAFE : OK) : EMPTY;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::read(event_t &event, size_t index) const {
  if (index < size)
    event = array[index];

  return index < size ? OK : OUT_OF_BOUND;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::write(event_t &event,
                                                             size_t index) {
  if (index < size)
    array[index] = event;

  return index < size ? OK : OUT_OF_BOUND;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::state(size_t &b,
                                                             size_t &t) const {
  ATOMIC_TRIPLE(b, t);

  DEBUGV3("b=0x%x t=0x%x\n", b, t);

  return OK;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::state(size_t &b, size_t &t, timespanw &ts,
                         timespanw &ts_t) const {
  ATOMIC_TRIPLE(b, t);

  ts = array[b].getTime();
  ts_t = array[t].getTime();

  DEBUGV3("timestamp=[0x%x,0x%x] b=0x%x t=0x%x\n", L(ts >> 32), L(ts), b, t);

  return OK;
}

template <typename T, size_t N> size_t RTML_buffer<T, N>::length() const {
  size_t b, t;
  state(b, t);
  return (t >= b) ? t - b : size - (b - t);
}

template <typename T, size_t N> void RTML_buffer<T, N>::debug() const {
  size_t b, t;
  timespanw ts, ts_t;
  state(b, t, ts, ts_t);

  DEBUGV3(
      "[STATE] bottom=0x%x top=0x%x timestamp=[0x%x,0x%x] timestamp=%luns\n", b,
      t, L(ts >> 32), L(ts), L(ts));

  DEBUGV3("[CONTENT]\n");
  DEBUGV3("----------\n");
  for (unsigned int idx = 0; idx < size + 2; idx++) {
    if (idx % 2 == 0)
      DEBUGV3_APPEND("\n");
    array[idx].debug();
  }

  DEBUGV3_APPEND("\n");
  DEBUGV3("__________\n");
}

template <typename T, size_t N>
RTML_buffer<T, N> &RTML_buffer<T, N>::operator=(const RTML_buffer<T, N> &rhs) {
  MEMCPY(this, &rhs, sizeof(rhs));
  return *this;
}

#endif //_RTML_CIRCULARBUFFER_H_
