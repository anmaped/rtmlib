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

#ifndef _RTEML_WRITER_H_
#define _RTEML_WRITER_H_

#include "circularbuffer.h"
#include "event.h"

#include "atomic_compat.h"

/**
 * Writes events to a RTML_buffer.
 *
 * @see RTML_buffer
 *
 * @author André Pedro
 * @date
 */
template <typename B> class RTML_writer {
private:
  /**
   * Points to a ring buffer this RTML_writer writes to.
   *
   * @see Ring_buffer
   */
  B &buffer;

  /**
   * the zeroed atomic page and id
   */
  ATOMIC_PAGE_SWAP();

  /**
   * Increment top of the writer
   */
  size_t &increment_writer_top(size_t &t) {
    if (++t >= buffer.size)
      t = 0;
    return t;
  };

  /**
   * Increment bottom of the writer
   */
  size_t &increment_writer_bottom(size_t &b) {
    if (++b >= buffer.size)
      b = 0;
    return b;
  };

public:
  /**
   * Instantiates a new RTML_writer.
   *
   * @param buffer the Buffer to write to.
   */
  RTML_writer(B &buffer);

  /**
   * push an event to the Buffer.
   *
   * @param data a constant reference to the data to be pushed.
   */
  typename B::error_t push(typename B::event_t &data);

  /**
   * force push an event to the Buffer.
   *
   * @param data a constant reference to the data to be pushed.
   */
  typename B::error_t push_all(typename B::event_t &data);
};

template <typename B>
RTML_writer<B>::RTML_writer(B &_buffer) : buffer(_buffer) {
  buffer.increment_writer();
}

template <typename B>
typename B::error_t RTML_writer<B>::push(typename B::event_t &event) {

  typename B::error_t err;

#if defined(__HW__)
#error "Please use push_all instead!"
#else
  size_t top;

  ATOMIC_PUSH({
    timespan timestamp = clockgettime();
    event.setTime(timestamp);

    top = stateref->top;
    increment_writer_top(stateref->top);

    bool p = stateref->top == stateref->bottom;

    if (p)
      increment_writer_bottom(stateref->bottom);

    err = (p) ? buffer.BUFFER_OVERFLOW : buffer.OK;
  });

  buffer.write(event, top);

#endif

  return err;
}

template <typename B>
typename B::error_t RTML_writer<B>::push_all(typename B::event_t &event) {

  typename B::error_t err;

#if defined(__HW__)
  err = buffer.push(event);
#else
  size_t top;

  ATOMIC_PUSH({
    top = stateref->top;
    increment_writer_top(stateref->top);

    bool p = stateref->top == stateref->bottom;

    if (p)
      increment_writer_bottom(stateref->bottom);

    err = (p) ? buffer.BUFFER_OVERFLOW : buffer.OK;
  });

  buffer.write(event, top);

#endif

  return err;
}

#endif //_RTEML_WRITER_H_
