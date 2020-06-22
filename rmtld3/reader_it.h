
#include "RTML_reader.h"

// defines an iterator for a trace
template<typename R>
class TraceIterator :  public std::iterator< std::input_iterator_tag, typename R::buffer_t::event_t::data_t >
{
  /*
   * Lets define the reader for communication with the rtmlib
   */
  R& __reader;

  size_t ibegin, iend, it;

  timespanw lower_abstime; // absolute time of the first iterator
  timespanw upper_abstime; // absolute time of the last iterator
  timespanw current_abstime; // absolute time of the current iterator

  bool enough; // a flag of enough elements for setbound function


  void setBegin(size_t b) { ibegin = b; }
  void setEnd(size_t e) { iend = e; }
  void setIt(size_t iter) { it = iter; }

  void setLowerAbsTime(timespanw lower_t) { lower_abstime = lower_t; }
  void setUpperAbsTime(timespanw upper_t) { upper_abstime = upper_t; }
  void setCurrentAbsTime(timespanw curr_t) { current_abstime = curr_t; }

  public:
    TraceIterator<R> (R& _l_reader,
      size_t i, size_t e, size_t iter,
      timespanw t_l, timespanw t_u, timespanw ct) :
      __reader(_l_reader),
      ibegin(i), iend(e), it(iter),
      lower_abstime(t_l), upper_abstime(t_u), current_abstime(ct),
	  enough(false)
    {};

    R& getReader() { return __reader; }

    size_t getBegin() { return ibegin; }
    size_t getIt() { return it; }
    bool getEnoughSize() { return enough; }

    timespanw getLowerAbsoluteTime() { return lower_abstime; }
    timespanw getUpperAbsoluteTime() { return upper_abstime; }
    timespanw getCurrentAbsoluteTime() { return current_abstime; }

    void setBound(std::tuple<timespanw, typename R::buffer_t::event_t, size_t, bool> lb,
      std::tuple<timespanw, typename R::buffer_t::event_t, size_t, bool> ub) {

      auto lb_absolute_idx = std::get<2>(lb);
      auto ub_absolute_idx = std::get<2>(ub) + 1;

      auto lower_abs_time = std::get<0>(lb);
      auto upper_abs_time = std::get<0>(ub);

      enough = std::get<3>(ub);

      setBegin( lb_absolute_idx );
      setIt( lb_absolute_idx );
      setEnd( ub_absolute_idx );

      // updates the timestamps
      setLowerAbsTime(lower_abs_time);
      setCurrentAbsTime(lower_abs_time);
      setUpperAbsTime(upper_abs_time);

      DEBUGV_RMTLD3("setBound: ");
      debug();
    }

    TraceIterator<R> begin() {
      return TraceIterator<R> (
        __reader,
        ibegin, iend, ibegin, lower_abstime, lower_abstime, upper_abstime );
    }

    TraceIterator<R> end() {
      return TraceIterator<R> (
        __reader,
        ibegin, iend, iend, upper_abstime, lower_abstime, upper_abstime);
    }

    TraceIterator<R>& operator++() {
      ++ it;

      // update absolute current time of the event
      current_abstime += operator*().getTime(); // [TODO] [VERIFY]

      return *this;
    } // [CONFIRM]

    bool operator==(const TraceIterator<R>& rhs) { return it == rhs.it; } // [CONFIRM]
    bool operator!=(const TraceIterator<R>& rhs) { return !(operator==(rhs)); } // [CONFIRM]

    typename R::buffer_t::event_t operator*() {

      // here we could adopt a small buffer to avoid successive call of dequeues (for instance a local buffer of 10 elements)
      // dequeue the event of the it index

      auto ev_it = it % __reader.getHigherIdx();

      std::pair<state_rd_t, typename R::buffer_t::event_t > x = __reader.dequeue(ev_it);

      return x.second;
    } // [CONFIRM]

    void debug()
    {
      DEBUGV_RMTLD3("It:%u %u %u %llu %llu %llu. pt:%p\n", ibegin, iend, it, lower_abstime, upper_abstime, current_abstime, __reader);
    }
};
