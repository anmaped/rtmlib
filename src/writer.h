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
  typename B::error_t push(typename B::event_t &);
};

template <typename B>
RTML_writer<B>::RTML_writer(B &_buffer) : buffer(_buffer) {}

template <typename B>
typename B::error_t RTML_writer<B>::push(typename B::event_t &event) {

  typename B::error_t err;

#if defined(__HW__)
  err = buffer.push(event);
#else
  size_t top;

  ATOMIC({
    timespan timestamp = clockgettime();
    event.setTime(timestamp);

    top = stateref->top;
    stateref->top = ((size_t)(stateref->top + 1) % (buffer.size + 1));

    bool p = stateref->top == stateref->bottom;

    if (p)
      stateref->bottom = ((size_t)(stateref->bottom + 1) % (buffer.size + 1));

    err = (p) ? buffer.OVERFLOW : buffer.OK;
  });

  buffer.write(event, top);
#endif

  return err;
}

#endif //_RTEML_WRITER_H_
