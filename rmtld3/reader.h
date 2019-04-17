#ifndef _RMTLD3_READER_H_
#define _RMTLD3_READER_H_

#include <functional>
#include <numeric>

#include "rmtld3.h"
#include "reader_it.h"

template<typename T, typename E>
class RMTLD3_reader : public RTML_reader<T>
{
  // current buffer state (idx and cnt)
  int state_cnt;
  size_t state_idx;

  timespanw formula_t_upper_bound;

  public:
    RMTLD3_reader(Ring_buffer<T> * const buffer, timespan ub) :
      RTML_reader<T>(buffer),
      formula_t_upper_bound(ub)
    {};

    /** match timestamp and return the index using forward direction (iterator++) */
    std::tuple<timespanw, E, size_t, bool> searchIndexForwardUntil(TraceIterator<T,E> &iterator, timespanw t) {

      timeabs current_time;
      timespanw aligned_time;
      size_t current_idx;

      std::pair<timeabs, size_t> pair_state =  iterator.getReader()->getCurrentBufferState();
      current_time = pair_state.first;
      current_idx = pair_state.second;

      aligned_time = iterator.getReader()->getTimeAlignment(current_time);

      DEBUGV_RMTLD3("    ENTER_search_index: c_t:%llu c_idx:%u\n", current_time, current_idx);

      /* create an iterator where its end is the endpoint of the buffer
       * and process the iteration until the timespanw t is found. After
       * that return the index and other data where t holds.
       */
      TraceIterator<T,E> tmp_it = TraceIterator<T,E>(
        iterator.getReader(),
        iterator.getIt(),                  // lower bound for index
        current_idx,                       // upper bound for index
        iterator.getIt(),                  // [TODO] this idx should be changed;

        iterator.getCurrentAbsoluteTime(), // set lower abs time to the old current abs time
        aligned_time,                      // set the new upper abs time
        iterator.getCurrentAbsoluteTime()  // begin iterator at lower abs time
      );

      //iterator.debug();
      //tmp_it.debug();

      /* lets start the iteration to find the event at time t.
       * the tuple consists of the cumulative timestamp, the event, the
       * absolute index, and a boolean flag indicating tat the event was found.
       * type is std::tuple<timespanw, E, size_t, bool>
       */
      auto event_find_tuple = std::accumulate(
        tmp_it.begin(),
        tmp_it.end(),
        std::make_tuple ( tmp_it.getCurrentAbsoluteTime(), E(), tmp_it.getIt(), false ),
          [t]( const std::tuple<timespanw, E, size_t, bool> a, E e )
          {
            timespanw time_lowerbound = std::get<0>(a);
            timespanw time_upperbound = time_lowerbound + e.getTime();

            bool found = std::get<3>(a);
            bool cdt = ( time_lowerbound <= t && t < time_upperbound );

            return std::make_tuple (
              (found || cdt)? time_lowerbound : time_upperbound, // time is always cumulating
              (found)? std::get<1>(a) : e, // old event or new event
              (found || cdt)? std::get<2>(a) : std::get<2>(a) + 1, // increment it or hold it
              (cdt)? true : found // the test for the existence of the event
            );
          }
      );

      //::printf(\"aligned_time:%llu found_time:%llu init_value(%llu)\n\", aligned_time, std::get<0>(event_find_tuple), tmp_it.getCurrentAbsoluteTime());

      // assert if there is no event found then timestamps should be equal
      ASSERT_RMTLD3( (std::get<3>(event_find_tuple)) || (aligned_time == std::get<0>(event_find_tuple)) );

      DEBUGV_RMTLD3("    EXIT_searchindex: find(%d) time(%llu) t=%llu idx(%d)\n", std::get<3>(event_find_tuple), std::get<0>(event_find_tuple), t, std::get<2>(event_find_tuple));

      

      return event_find_tuple;
    }; // [TODO]

    // pre-indexed search
    three_valued_type searchOForward(std::pair <size_t, timespanw> &state, proposition p, timespan t) {

      // construct iterator based on the state [TODO]
      // use env.state to speedup the calculation of the new bounds
      TraceIterator<T,E> it = TraceIterator<T,E> (this, state.first, 0, state.first, state.second, 0, state.second );

      DEBUGV_RMTLD3("  searchOForward: ");
      //it.debug();

      /* use the function searchIndexForwardUntil to find the index where
       * t hold and check if proposition is satisfied at time t
       */
      // getting the index of current t
      auto event_tuple = searchIndexForwardUntil( it, t );

      if( !std::get<3>(event_tuple) ) // is it not a valid event?
      {
        timespanw current_time = std::get<0>(event_tuple);
        /* here we require to distinguish the Unknown value due to the unknown trace
         * if the bound does not hold than return false
         */
        if ( current_time > formula_t_upper_bound )
        {
          // the bound is violated here; raise error message
          DEBUGV_RMTLD3("monitor overloads with %llu > %llu\n", current_time, formula_t_upper_bound);
          return T_FALSE;
        }
        else
        {
          //DEBUGV_RMTLD3(\"    Unknown\n\");
          return T_UNKNOWN;
        }
      }
      
      proposition new_p = std::get<1>(event_tuple).getData();

      DEBUGV_RMTLD3("  end searchOForward\n");

      if(new_p == p)
      {
        //DEBUGV_RMTLD3(\"    True\n\");
        return T_TRUE;
      }
      else
      {
        //DEBUGV_RMTLD3(\"    False\n\");
        return T_FALSE;
      }

    }; // [TODO]

    three_valued_type searchOBackward(std::pair <size_t, timespanw> &state, proposition p, timespan t) { return T_FALSE; }; // [TODO]

};

template<typename T, typename E>
struct Environment {
  std::pair <size_t, timespanw> state;
  RMTLD3_reader< T, E > * const trace;
  three_valued_type (*const evaluate)(struct Environment<T,E> &, proposition, timespan);

  Environment<T,E>(
    std::pair <size_t, timespanw> st,
    RMTLD3_reader< T, E > * const t,
    three_valued_type (*const ev)(struct Environment<T,E> &, proposition, timespan)
  ) :
    state(st),
    trace(t),
    evaluate(ev) {};
};

// obs lambda function
template<typename T, typename E>
auto __observation = []( struct Environment<T,E> &env, proposition p, timespan t) mutable -> three_valued_type
{
  DEBUGV_RMTLD3("  eval: %lu prop:%d\n", t, p);
  return b3_or ( env.trace->searchOForward(env.state, p, t), env.trace->searchOBackward(env.state, p, t) );
};


#endif //_RMTLD3_READER_H_
