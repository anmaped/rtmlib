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

#ifndef _RMTLD3_READER_H_
#define _RMTLD3_READER_H_

#include <numeric>

#include "pattern.h"
#include "rmtld3.h"

template <typename R, typename P = size_t> class RMTLD3_reader : public R {

  /**
   * Cursor between bottom and top of the reader
   */
  size_t cursor;

public:
  /**
   *  Local memory for dynamic programming pattern
   */
  P &lmem;

  /**
   * Constructor
   */
  RMTLD3_reader(const typename R::buffer_t &_buffer)
      : R(_buffer), lmem(cursor), cursor(0){};
  RMTLD3_reader(const typename R::buffer_t &_buffer, P &_lmem)
      : R(_buffer), lmem(_lmem), cursor(0){};

  /**
   * Resets cursor in the reader
   */
  typename R::error_t reset();

  /**
   * Sets cursor at time t
   */
  typename R::error_t set(timespan &);

  /**
   * Sets cursor
   */
  typename R::error_t set_cursor(size_t &);

  /**
   * Gets cursor
   */
  size_t &get_cursor();

  /**
   * Increment current cursor
   */
  typename R::error_t increment_cursor();

  /**
   * Decrement current cursor
   */
  typename R::error_t decrement_cursor();

  /**
   * Pull event
   */
  typename R::error_t pull(typename R::buffer_t::event_t &);

  /**
   * Read event without removing it from the buffer
   */
  typename R::error_t read(typename R::buffer_t::event_t &);

  /**
   * Read next event without removing it from the buffer
   */
  typename R::error_t read_next(typename R::buffer_t::event_t &);

  /**
   * Read previous event without removing it from the buffer
   */
  typename R::error_t read_previous(typename R::buffer_t::event_t &);

  /**
   * Current buffer length of the reader
   */
  size_t length() const;

  /**
   * Number of reader's consumed elements from the buffer
   */
  size_t consumed() const;

  /**
   * Enable debug message for reader
   */
  void debug() const;
};

template <typename R, typename P>
typename R::error_t RMTLD3_reader<R, P>::reset() {

  cursor = R::bottom;

  return R::AVAILABLE;
}

template <typename R, typename P>
typename R::error_t RMTLD3_reader<R, P>::set(timespan &t) {

  typename R::buffer_t::event_t e, ee;

  while (read_previous(e) == R::AVAILABLE && read(ee) == R::AVAILABLE) {

    DEBUGV_RMTLD3("--- %lu %lu\n", e.getTime(), ee.getTime());

    if (e.getTime() <= t && t < ee.getTime()) {
      return R::AVAILABLE;
    }

    decrement_cursor();
    DEBUGV_RMTLD3("backward cursor=%d\n", cursor);
  }

  while (read(ee) == R::AVAILABLE && read_next(e) == R::AVAILABLE) {

    DEBUGV_RMTLD3("--- %lu %lu\n", ee.getTime(), e.getTime());

    if (ee.getTime() <= t && t < e.getTime()) {
      return R::AVAILABLE;
    }

    increment_cursor();
    DEBUGV_RMTLD3("forward cursor=%d\n", cursor);
  }

  return R::UNAVAILABLE;
}

template <typename R, typename P>
typename R::error_t RMTLD3_reader<R, P>::set_cursor(size_t &c) {

  typename R::buffer_t::event_t e;

  // to check the validity of the cursor before changing it
  if (R::buffer.read(e, c) == R::buffer.OK) {
    cursor = c;
    return R::AVAILABLE;
  } else {
    DEBUG_RMTLD3("cursor set %li is not available\n", c);
    return R::UNAVAILABLE;
  }
}

template <typename R, typename P> size_t &RMTLD3_reader<R, P>::get_cursor() {
  return cursor;
}

template <typename R, typename P>
typename R::error_t RMTLD3_reader<R, P>::increment_cursor() {

  // check if current cursor is bounded between bot and top
  if (R::top == cursor)
    return R::UNAVAILABLE;

  // cursor = (size_t)(cursor + 1) % R::buffer.size;
  if (++cursor >= R::buffer.size)
    cursor = 0;

  return R::AVAILABLE;
}

template <typename R, typename P>
typename R::error_t RMTLD3_reader<R, P>::decrement_cursor() {

  // check if current cursor is bounded between bot and top
  if (R::bottom == cursor)
    return R::UNAVAILABLE;

  // cursor = (size_t)(cursor - 1) % R::buffer.size;
  if (cursor == 0)
    cursor = R::buffer.size - 1;
  else
    --cursor;

  return R::AVAILABLE;
}

template <typename R, typename P>
typename R::error_t
RMTLD3_reader<R, P>::pull(typename R::buffer_t::event_t &e) {

  typename R::buffer_t::event_t event_next;

  if (length() > 0) {
    // read in an atomic way
    if (R::buffer.read(e, cursor) != R::buffer.OK)
      return R::READ_ERROR;

    increment_cursor();
  }

  DEBUGV_RMTLD3("pull-> %d (%d,%d)\n", length(), cursor, R::top);

  return (length() > 0) ? R::AVAILABLE : R::UNAVAILABLE;
}

template <typename R, typename P>
typename R::error_t
RMTLD3_reader<R, P>::read(typename R::buffer_t::event_t &e) {

  return (R::buffer.read(e, cursor) == R::buffer.OK) ? R::AVAILABLE
                                                     : R::UNAVAILABLE;
}

template <typename R, typename P>
typename R::error_t
RMTLD3_reader<R, P>::read_next(typename R::buffer_t::event_t &e) {

  return ((length() > 1 &&
           cursor != R::top && // [TODO: check this: !(cursor == R::top)]
           (R::buffer.read(e, ((cursor + 1) >= R::buffer.size) ? 0
                                                               : cursor + 1)) ==
               R::buffer.OK))
             ? R::AVAILABLE
             : R::UNAVAILABLE;
}

template <typename R, typename P>
typename R::error_t
RMTLD3_reader<R, P>::read_previous(typename R::buffer_t::event_t &e) {

  DEBUGV_RMTLD3("consumed=%d available=%d total=%d\n", consumed(), length(),
                R::length());
  return ((consumed() > 0 &&
           cursor != R::bottom && // [TODO: check this: !(cursor == R::bottom)]
           (R::buffer.read(e, ((cursor - 1) < 0) ? R::buffer.size - 1
                                                 : cursor - 1)) ==
               R::buffer.OK))
             ? R::AVAILABLE
             : R::UNAVAILABLE;
}

template <typename R, typename P> size_t RMTLD3_reader<R, P>::length() const {
  return (R::top >= cursor) ? R::top - cursor
                            : R::buffer.size - (cursor - R::top);
}

template <typename R, typename P> size_t RMTLD3_reader<R, P>::consumed() const {
  return R::length() - length();
}

template <typename R, typename P> void RMTLD3_reader<R, P>::debug() const {

  typename R::buffer_t::event_t e;
  for (size_t i = 0; i < R::buffer.size; i++) {
    R::buffer.read(e, i);
    DEBUGV_RMTLD3("(%d,%lu),", e.getData(), e.getTime());
  }
  DEBUGV_RMTLD3("\n");
  DEBUGV_RMTLD3("bottom=%d top=%d timestamp=%lu | cursor=%d \n", R::bottom,
                R::top, R::timestamp, cursor);
}

#endif //_RMTLD3_READER_H_
