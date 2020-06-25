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

#ifndef _RTEML_READER_H_
#define _RTEML_READER_H_

#include <array>
#include <time.h>
#include <tuple>
#include <utility>

#include "RTML_event.h"
#include "RTML_buffer.h"

/**
 * Reader to support local RTML_buffer management.
 *
 * @author André Pedro
 * @date
 */
template <typename B> class RTML_reader {
private:
  /**  Constant pointer to a ring Buffer this RTML_reader reads from.
   * @see RTML_buffer
   */
  const B &buffer;

  /**
   * The reader contains a top and a bottom
   */
  size_t top;

  /**
   * The bottom of the reader
   */
  size_t bottom;

  /**
   * The timestamp of the reader to detect gaps
   */
  timespanw timestamp;

public:
  typedef B buffer_t;

  typedef enum { AVAILABLE = 0, UNAVAILABLE, OVERFLOW, BUFFER_READ } error_t;

  /**
   * Constructs a new RTML_reader.
   *
   * Construct the new event reader that reads and manage events from a buffer.
   *
   * @param buffer a constant pointer that points to the buffer.
   */
  RTML_reader(const B &);

  /**
   * Pull event from the buffer.
   *
   * @return an errot_t
   *
   */
  error_t pull(typename B::event_t &);

  /**
   * Pop event from the buffer.
   *
   * @return an errot_t
   *
   */
  error_t pop(typename B::event_t &);

  /**
   * Synchronizes the RTML_reader with the buffer
   *
   * @return true if the RTML_reader have been synchronized, false otherwise.
   */
  bool synchronize();

  /**
   * Detects a gap. It compares the current RTML_reader absolute timestamp with
   * the current absolute timestamp of the buffer.
   *
   * @return true if there is a gap in the buffer, false otherwise.
   */
  bool gap() const;

  /**
   * Get reader length
   *
   * @return the template parameter N.
   */
  size_t length() const;
};

template <typename B>
RTML_reader<B>::RTML_reader(const B &_buffer)
    : buffer(_buffer), top(0), bottom(0), timestamp(0) {}

template <typename B>
typename RTML_reader<B>::error_t
RTML_reader<B>::pull(typename B::event_t &event) {

  if (length() > 0) {
    // read in an atomic way
    if (buffer.read(event, bottom) != buffer.OK)
      return BUFFER_READ;

    // update local bottom
    bottom = (size_t)(bottom + 1) % (buffer.size + 0);
  }

  DEBUGV3("pull-> %d (%d,%d)\n", length(), bottom, top);

  if (gap())
    return OVERFLOW;
  else
    return (length() > 0) ? AVAILABLE : UNAVAILABLE;
}

template <typename B>
typename RTML_reader<B>::error_t
RTML_reader<B>::pop(typename B::event_t &event) {

  if (length() > 0) {
    if (top - 1 > 0)
      buffer.read(event, --top);
    else if (top - 1 <= 0) {
      top = buffer.size;
      buffer.read(event, top);
    }
  }

  DEBUGV3("pop-> %d (%d,%d)\n", length(), bottom, top);

  if (gap())
    return OVERFLOW;
  else
    return (length() > 0) ? AVAILABLE : UNAVAILABLE;
}

template <typename B> bool RTML_reader<B>::synchronize() {
  /*
   * The synchronization depends on the buffer state. Any event that is
   * overwritten without being read is a gap for the reader.
   */

  size_t b, t;
  timespanw ts;
  buffer.state(b, t, ts);

  if (timestamp >= ts) {
    // no gaps, but the reader may be further ahead
    return false;
  } else {
    // update reader state
    bottom = b;
    top = t;
    timestamp = ts;

    // there is a gap
    return true;
  }
}

template <typename B> bool RTML_reader<B>::gap() const {
  typename B::event_t event;

  // detect a gap
  buffer.read(event, bottom);
  if (timestamp < event.getTime())
    return true;

  return false;
}

template <typename B> size_t RTML_reader<B>::length() const {
  return (top >= bottom) ? top - bottom : (buffer.size + 1) - (bottom - top);
}

#endif //_RTEML_READER_H_
