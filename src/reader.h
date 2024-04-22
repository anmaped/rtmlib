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

#include "circularbuffer.h"
#include "event.h"

/**
 * Reader to support local RTML_buffer management.
 *
 * @author André Pedro
 * @date
 */
template <typename B> class RTML_reader {
protected:
  /**  Constant reference to a ring Buffer this RTML_reader reads from.
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

  /**
   * Decrement top of the reader
   */
  size_t &decrement_reader_top() {
    if (top == 0)
      top = buffer.size - 1;
    else
      --top;
    return top;
  };

  /**
   * Increment bottom of the reader
   */
  size_t &increment_reader_bottom() {
    if (++bottom >= buffer.size)
      bottom = 0;
    return bottom;
  };

  /**
   * Decrement bottom of the reader
   */
  size_t &decrement_reader_bottom() {
    if (bottom == 0)
      bottom = buffer.size - 1;
    else
      --bottom;
    return bottom;
  };

public:
  typedef B buffer_t;

  typedef enum {
    AVAILABLE = 0,
    UNAVAILABLE,
    READER_OVERFLOW,
    READ_ERROR
  } error_t;

  typedef enum { NO_GAP = 0, GAP, UNKNOWN_GAP } gap_error_t;

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
  gap_error_t synchronize();

  /**
   * Detects a gap. It compares the current RTML_reader absolute timestamp with
   * the current absolute timestamp of the buffer.
   *
   * @return true if there is a gap in the buffer, false otherwise.
   */
  gap_error_t gap() const;

  /**
   * Get reader length
   *
   * @return the template parameter N.
   */
  size_t length() const;

  /**
   * Debug function
   */
  void debug() const;
};

template <typename B>
RTML_reader<B>::RTML_reader(const B &_buffer)
    : buffer(_buffer), top(0), bottom(0), timestamp(0) {}

template <typename B>
typename RTML_reader<B>::error_t
RTML_reader<B>::pull(typename B::event_t &event) {

  typename B::event_t event_next;
  const int delta = 1;

  if (length() > delta) {
    // read in an atomic way
    if (buffer.read(event, bottom) != buffer.OK)
      return READ_ERROR;

    // update local bottom
    increment_reader_bottom();

    DEBUGV3("pull-> length=%lu bottom=%lu top=%lu\n", length(), bottom, top);

    // use last known timestamp (delta bigger than one may help to find the
    // right timestamp)
    if (buffer.read(event_next, bottom) == buffer.OK) {
      timestamp = event_next.getTime();

      if (gap())
        return READER_OVERFLOW;
      else
        return AVAILABLE;
    } else {
      decrement_reader_bottom();
      return READ_ERROR;
    }
  }

  return UNAVAILABLE;
}

template <typename B>
typename RTML_reader<B>::error_t
RTML_reader<B>::pop(typename B::event_t &event) {

  if (length() > 0) {
    decrement_reader_top();
    if (buffer.read(event, top) != buffer.OK)
      return READ_ERROR;

    if (gap())
      return READER_OVERFLOW;
    else
      return AVAILABLE;
  }

  DEBUGV3("pop-> %d (%d,%d)\n", length(), bottom, top);

  return UNAVAILABLE;
}

template <typename B>
typename RTML_reader<B>::gap_error_t RTML_reader<B>::synchronize() {
  /*
   * The synchronization depends on the buffer state. Any event that is
   * overwritten without being read is identified as a gap for the reader.
   */

  size_t b, t;
  timespanw ts, ts_t;
  buffer.state(b, t, ts, ts_t);

  DEBUGV("reader ts:%lu buffer ts:%lu\n", timestamp, ts);

  /*
   *
   * Example:
   *
   * ts(reader.bottom) < ts(buffer.b) > ts(buffer.t)
   * 7                   9              8
   * (t < b) implies (0 < ts(bottom) < ts(b)) or (ts(t) < ts(bottom)  <
   *  buffer.size)
   *
   * (b < t) implies (ts(b) < ts(bottom) < ts(t))
   *
   * (t == b) should not happen
   *
   */

  DEBUGV("sync: %d %d %d %d %d | (%d,%d) %d %d\n", t < b, b < t,
         0 < timestamp && timestamp < ts, ts_t < timestamp,
         ts < timestamp && timestamp < ts_t, bottom, top, b, t);

  if (t < b) {
    DEBUGV("1-> t < b (with overlap)\n");
    if ((0 < timestamp && timestamp < ts) || (ts_t < timestamp)) {
      /*
       * This can be triggered in the begining to syncronize the reader for the
       * first time.
       */
      if (gap() == GAP) {
        top = t;
        bottom = b;
        timestamp = ts;
        return GAP;
      }
      // update top (no gap found)
      top = t;
      return NO_GAP; // SOFT_SYNC
    } else {
      // reader is out of sync
      top = t;
      bottom = b;
      timestamp = ts;

      return GAP; // HARD_SYNC
    }
  } else if (b < t) {
    DEBUGV("2-> b < t (without overlap)\n");
    if (ts < timestamp && timestamp < ts_t) {
      // update top (no gap found)
      top = t;

      return NO_GAP; // SOFT_SYNC
    } else {
      // reader is out of sync
      top = t;
      bottom = (b < bottom && b < t) ? bottom : b;
      timestamp = (b < bottom && b < t) ? timestamp : ts;

      return GAP; // HARD_SYNC
    }
  } else {
    DEBUGV("3-> b == t (initial case)\n");
    /*
     * This can also be triggered in the begining to syncronize the reader for
     * the first time.
     */
    if (top == 0 && bottom == 0) {
      top = t;
      bottom = b;
      timestamp = ts;

      return GAP; // INIT SYNC
    } else {
      // this case should not happen
      DEBUGV("## unknown gap ## t == b && !(top == 0 && bottom == 0)\n");
      return UNKNOWN_GAP;
    }
  }
}

template <typename B>
typename RTML_reader<B>::gap_error_t RTML_reader<B>::gap() const {
  typename B::event_t event;

  // detect a gap
  if (buffer.read(event, bottom) == buffer.OK) {
    if (timestamp < event.getTime())
      return GAP;
  } else {
    // this case should not happen
    DEBUGV("## unknown gap ## buffer read!\n");
    return UNKNOWN_GAP;
  }

  return NO_GAP;
}

template <typename B> size_t RTML_reader<B>::length() const {
  return (top >= bottom) ? top - bottom : (buffer.size) - (bottom - top);
}

template <typename B> void RTML_reader<B>::debug() const {
  DEBUGV("bottom:%lu top:%lu timestamp:%lli\n", bottom, top, timestamp);
}

#endif //_RTEML_READER_H_
