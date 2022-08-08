#ifndef _RMTLD3_READER_H_
#define _RMTLD3_READER_H_

#include <numeric>

#include "pattern.h"
#include "rmtld3.h"

template <typename R, typename P> class RMTLD3_reader : public R {

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
  RMTLD3_reader(const typename R::buffer_t &_buffer, P &_lmem)
      : R(_buffer), lmem(_lmem), cursor(0){};

  /**
   * Resets cursor in the reader
   */
  typename R::buffer_t::error_t reset();

  /**
   * Sets cursor at time t
   */
  typename R::buffer_t::error_t set(timespan &);

  typename R::error_t pull(typename R::buffer_t::event_t &);

  typename R::error_t read(typename R::buffer_t::event_t &);

  typename R::error_t read_next(typename R::buffer_t::event_t &);

  typename R::error_t read_previous(typename R::buffer_t::event_t &);

  size_t length() const;

  size_t consumed() const;

  void debug() const;
};

template <typename R, typename P>
typename R::buffer_t::error_t RMTLD3_reader<R, P>::reset() {

  cursor = R::bottom;

  return R::buffer_t::OK;
}

template <typename R, typename P>
typename R::buffer_t::error_t RMTLD3_reader<R, P>::set(timespan &t) {

  typename R::buffer_t::event_t e;

  while (read_previous(e) == R::AVAILABLE) {

    if (t > e.getTime())
      break;

    // set cursor back
    cursor = (size_t)(cursor - 1) % (R::buffer.size + 0);
    DEBUGV_RMTLD3("backtrack cursor=%d\n", cursor);
  }

  return R::buffer_t::OK;
}

template <typename R, typename P>
typename R::error_t
RMTLD3_reader<R, P>::pull(typename R::buffer_t::event_t &e) {

  typename R::buffer_t::event_t event_next;

  if (length() > 0) {
    // read in an atomic way
    if (R::buffer.read(e, cursor) != R::buffer.OK)
      return R::BUFFER_READ;

    // update local cursor
    cursor = (size_t)(cursor + 1) % (R::buffer.size + 0);
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
           (R::buffer.read(e, (size_t)(cursor + 1) % (R::buffer.size + 0))) ==
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
           (R::buffer.read(e, (size_t)(cursor - 1) % (R::buffer.size + 0))) ==
               R::buffer.OK))
             ? R::AVAILABLE
             : R::UNAVAILABLE;
}

template <typename R, typename P> size_t RMTLD3_reader<R, P>::length() const {
  return (R::top >= cursor) ? R::top - cursor
                            : (R::buffer.size + 1) - (cursor - R::top);
}

template <typename R, typename P> size_t RMTLD3_reader<R, P>::consumed() const {
  return R::length() - length();
}

template <typename R, typename P> void RMTLD3_reader<R, P>::debug() const {

  typename R::buffer_t::event_t e;
  for (int i = 0; i < R::buffer.size; i++) {
    R::buffer.read(e, i);
    DEBUGV_RMTLD3("(%d,%d),", e.getData(), e.getTime());
  }
  DEBUGV_RMTLD3("\n");
  DEBUGV_RMTLD3("bottom=%d top=%d timestamp=%d | cursor=%d \n", R::bottom,
                R::top, R::timestamp, cursor);
}

#endif //_RMTLD3_READER_H_
