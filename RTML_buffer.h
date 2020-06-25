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

#ifndef _RTML_BUFFER_H_
#define _RTML_BUFFER_H_

#include <stdio.h>
//#include <time.h>

#include "RTML_event.h"
#include "debug_compat.h"

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
   * The Event buffer where events are kept. Size is defined via template
   * parameter N which avoids dynamic memory usage.
   *
   * @see Event
   */
  Event<T> array[N + 1];

  /**
   * The circular buffer contains a top and a bottom
   */
  size_t top;

  /**
   * The bottom of the circular buffer
   */
  size_t bottom;

  /**
   * The writer flag that indicates if a writer has been attached.
   */
  bool writer;

  /**
   * Increment top of the buffer (++ is just a mnemonic)
   */
  size_t &operator++() {
    top = (size_t)(top + 1) % (N + 1);
    return top;
  };

  /**
   * Increment top of the buffer (++ is just a mnemonic)
   */
  size_t operator++(int) {
    size_t tmp(top);
    operator++();
    return tmp;
  };

  /**
   * Increment bottom of the buffer (-- is just a mnemonic)
   */
  size_t &operator--() {
    bottom = (size_t)(bottom + 1) % (N + 1);
    return bottom;
  };

  /**
   * Increment bottom of the buffer (-- is just a mnemonic)
   */
  size_t operator--(int) {
    size_t tmp(bottom);
    operator--();
    return tmp;
  };

public:
  const size_t size = N;

  typedef Event<T> event_t;

  typedef enum { OK = 0, EMPTY, OVERFLOW, OUT_OF_BOUND } error_t;

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
   * Get node at index
   */
  error_t read(event_t &, size_t) const;

  /**
   *
   */
  error_t state(size_t &, size_t &, timespanw &) const;

  /**
   * Get buffer length
   *
   * @return the template parameter N.
   */
  size_t length() const;

  /**
   * Print buffer into the stdout
   */
  void debug() const;
};

template <typename T, size_t N>
RTML_buffer<T, N>::RTML_buffer() : top(0), bottom(0), array(), writer(false) {}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::push(const Event<T> &node) {

  array[top] = node;

  ++(*this);

  bool p = top == bottom;

  if (p)
    --(*this); // discard one element; the buffer is under a gap

  DEBUGV3("push-> %d (%d,%d) r:%d\n", length(), bottom, top, p);

  return (p) ? OVERFLOW : OK;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::pull(Event<T> &event) {
  bool c = length() > 0;
  if (c) {
    event = array[bottom];
    --(*this);
  }

  DEBUGV3("pull-> %d (%d,%d) r:%d e:%d\n", length(), bottom, top, c,
          event.getTime());

  return c ? OK : EMPTY;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t RTML_buffer<T, N>::pop(Event<T> &event) {
  bool c = length() > 0;
  if (c) {
    if (((int)top) - 1 >= 0) {
      event = array[--top];
    } else {
      top = N;
      event = array[top];
    }
  }

  DEBUGV3("pop-> %d (%d,%d) r:%d e:%d\n", length(), bottom, top, c,
          event.getTime());

  return c ? OK : EMPTY;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::read(Event<T> &event, size_t index) const {
  event = array[index];

  return index < N + 1 ? ((length() > 0) ? OK : EMPTY) : OUT_OF_BOUND;
}

template <typename T, size_t N>
typename RTML_buffer<T, N>::error_t
RTML_buffer<T, N>::state(size_t &b, size_t &t, timespanw &ts) const {
  b = bottom;
  t = top;
  ts = array[bottom].getTime();

  return OK;
}

template <typename T, size_t N> size_t RTML_buffer<T, N>::length() const {
  return (top >= bottom) ? top - bottom : (N + 1) - (bottom - top);
}

template <typename T, size_t N> void RTML_buffer<T, N>::debug() const {
  DEBUGV3(" ");
  for (unsigned int idx = 0; idx < N + 1; idx++)
    DEBUGV3_APPEND("%lu,%d; ", array[idx].ev.getTime(),
                   array[idx].ev.getData());
  DEBUGV3_APPEND("\n");
}

#endif //_RTML_BUFFER_H_
