#ifndef _RMTLD3_READER_H_
#define _RMTLD3_READER_H_

#include <functional>
#include <numeric>

#include "rmtld3.h"

template <typename R> class RMTLD3_reader : public R {

  size_t cursor;

  timespanw upper_bound;

public:
  RMTLD3_reader(const typename R::buffer_t &_buffer, timespan ub)
      : R(_buffer), cursor(0), upper_bound(ub){};

  /**
   * Resets cursor in the reader
   */
  typename R::buffer_t::error_t reset();

  /**
   * Sets cursor at time t
   */
  typename R::buffer_t::error_t set(timespan &);

  typename R::buffer_t::error_t read(typename R::buffer_t::event_t &);

  void debug() const;

};

template <typename R> typename R::buffer_t::error_t RMTLD3_reader<R>::reset() {

  // [TODO]
  // Synchronize cursor with buffer end_point

  return R::buffer_t::OK;
}

template <typename R> typename R::buffer_t::error_t RMTLD3_reader<R>::set(timespan &t) {

  //typename R::event_t previous_event, next_event;
  //R::buffer.prev(previous_event);
  //R::buffer.next(next_event);

  /*while () {
    if (timestamp <= t && previous_event.getTime() <= t &&
        t < next_event.getTime()) {
      return OK;
    } else {
      continue;
    }
  }*/

  // [TODO]
  // set cursos into time t from the current cursor position

  return R::buffer_t::OK;
}

template <typename R> typename R::buffer_t::error_t RMTLD3_reader<R>::read(typename R::buffer_t::event_t &e) {

  // [TODO]
  // Synchronize cursor with buffer end_point

  return R::buffer.read(e,cursor);

}

template <typename R> void RMTLD3_reader<R>::debug() const {

	typename R::buffer_t::event_t e;
	for(int i=0; i< R::buffer.size; i++)
	{
		R::buffer.read(e,i);
		DEBUGV_RMTLD3("(%d,%d),", e.getData(), e.getTime());
	}
	DEBUGV_RMTLD3("\n");
	DEBUGV_RMTLD3("bottom=%d top=%d timestamp=%d | cursor=%d upper_bound=%d\n", R::bottom, R::top, R::timestamp, cursor, upper_bound);
}


#endif //_RMTLD3_READER_H_
