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

/**
 * This header file contain the template to construct the RMTLD3 formulas.
 */

#include <utility>

#include "rmtld3.h"
#include "terms.h"

typedef bool with_in_between;

/**
 * Proposition
 */
template <typename T>
three_valued_type prop(T &trace, const proposition &p, timespan &t) {

  typename T::buffer_t::event_t e;
  typename T::buffer_t::event_t e_next;

  typename T::error_t status = trace.read(e);

  if (status == trace.AVAILABLE && trace.read_next(e_next) == trace.AVAILABLE &&
      e.getTime() <= t && t < e_next.getTime()) {
    DEBUGV_RMTLD3("eval: t=%lu prop=%d e=(%d %d) e_next=(%d %d)\n", t, p,
                  e.getData(), e.getTime(), e_next.getData(), e_next.getTime());
    return e.getData() == p ? T_TRUE : T_FALSE;
  } else {
    DEBUGV_RMTLD3("eval: t=%lu prop=%d e=(%d %d) unbounded\n", t, p,
                  e.getData(), e.getTime());
    return T_UNKNOWN;
  }
}

/**
 * Until (<)
 */
template <typename T, typename E, timespan b, typename S = void>
three_valued_type until_less(T &trace, timespan &t) {
  auto eval_fold = [](T &trace,
                      timespan &t) -> std::pair<four_valued_type, timespan> {
    // eval_b lambda function
    auto eval_b = [](T &trace, timespan &t,
                     four_valued_type &v) -> four_valued_type {
      // eval_i lambda function
      auto eval_i = [](three_valued_type &b1,
                       three_valued_type &b2) -> four_valued_type {
        return (b2 != T_FALSE) ? b3_to_b4(b2)
                               : ((b1 != T_TRUE) ? b3_to_b4(b1) : FV_SYMBOL);
      };

      size_t c = trace.get_cursor();
      DEBUGV_RMTLD3("$compute phi1\n");
      // compute phi1
      three_valued_type cmpphi1 = E::eval_phi1(trace, t);
      DEBUGV_RMTLD3("@compute phi1.\n");
      trace.set_cursor(c); // reset the cursor changes during the evaluation of
                           // the subformula

      c = trace.get_cursor();
      DEBUGV_RMTLD3("$compute phi2\n");
      // compute phi2
      three_valued_type cmpphi2 = E::eval_phi2(trace, t);
      DEBUGV_RMTLD3("@compute phi2.\n");
      trace.set_cursor(c); // reset the cursor changes during the evaluation of
                           // the subformula

      four_valued_type rs = eval_i(cmpphi1, cmpphi2);

      DEBUGV_RMTLD3("eval_b: phi1=%s phi2=%s\n", out_p(cmpphi1),
                    out_p(cmpphi2));

      if (v == FV_SYMBOL) {
        return rs;
      } else {
        return v;
      }
    };

    timespan c_time = t;

    four_valued_type symbol = FV_SYMBOL;

    typename T::buffer_t::event_t event;

    do {
      DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

      trace.read(event);
      c_time = event.getTime();

      if (c_time >= b + t) /* Check: > or >= */
        break;

      symbol = eval_b(trace, c_time, symbol);

      trace.debug();

      // check in-between; enable it when needed
      constexpr const int v = std::is_void<S>::value;
      if (v == false) {
        size_t store_cursor = trace.get_cursor();
        timespan t1, t2, t3 = c_time;
        typename T::buffer_t::event_t e1, e2;
        while (true) {
          trace.increment_cursor();
          if (trace.length() <= 1 || trace.read(e1) != trace.AVAILABLE ||
              trace.read_next(e2) != trace.AVAILABLE) {
            break;
          }

          t1 = e1.getTime();
          t2 = e2.getTime();
          t3 += (t2 - t1);

          if (t3 >= t1)
            break;

          DEBUGV_RMTLD3("t1=%d t2=%d t3=%d\n", t1, t2, t3);

          size_t store_cursor2 = trace.get_cursor();
          trace.set(t3);
          trace.decrement_cursor();
          symbol = eval_b(trace, t3, symbol);
          trace.set_cursor(store_cursor2);
        };
        trace.set_cursor(store_cursor);
        // end check in-between
      }

      // check if symbol converged and stop!
      if (symbol != FV_SYMBOL)
        break;

    } while (trace.pull(event) == trace.AVAILABLE);

    return std::make_pair(symbol, c_time);
  };

  size_t c_until = trace.get_cursor();

  DEBUGV_RMTLD3("$(+++) until_op_less\n");

  std::pair<four_valued_type, timespan> eval_c = eval_fold(trace, t);

  DEBUGV_RMTLD3("@(---) until_op (%s) enough(%d)=%d.\n", out_fv(eval_c.first),
                eval_c.second, eval_c.second < t + b);

  trace.set_cursor(c_until); // reset the cursor changes during the evaluation
                             // of the subformula

  return (eval_c.first == FV_SYMBOL)
             ? ((eval_c.second < t + b) ? T_UNKNOWN : T_FALSE)
             : b4_to_b3(eval_c.first);
}

/**
 * Eventually(=)
 *
 */
template <typename T, typename E, timespan b>
three_valued_type eventually_equal(T &trace, timespan &t) {

  timespan c_time = t;
  typename T::buffer_t::event_t event;
  three_valued_type symbol = T_UNKNOWN;

  size_t c_eventually = trace.get_cursor();

  do {
    DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

    trace.read(event);
    c_time = event.getTime();

    if (c_time > b + t)
      break;

    size_t c = trace.get_cursor();
    DEBUGV_RMTLD3("$compute phi\n");
    symbol = E::eval_phi1(trace, c_time);
    DEBUGV_RMTLD3("@compute phi.\n");
    trace.set_cursor(
        c); // reset the cursor changes during the evaluation of the subformula

    trace.debug();
  } while (trace.pull(event) == trace.AVAILABLE);

  trace.set_cursor(c_eventually); // reset the cursor changes during the
                                  // evaluation of the subformula

  return symbol;
}

/**
 * Eventually(<) Unbounded
 *
 */
template <typename T, typename E>
three_valued_type eventually_less_unbounded(T &trace, timespan &t) {

  timespan c_time = t;
  typename T::buffer_t::event_t event;
  three_valued_type symbol = T_FALSE;

  size_t c_eventually = trace.get_cursor();

  while (true) {
    DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

    trace.read(event);
    c_time = event.getTime();

    size_t c = trace.get_cursor();
    DEBUGV_RMTLD3("$compute phi\n");
    symbol = E::eval_phi1(trace, c_time);
    DEBUGV_RMTLD3("@compute phi.\n");
    trace.set_cursor(
        c); // reset the cursor changes during the evaluation of the subformula

    DEBUGV_RMTLD3("eval: phi1=%s\n", out_p(symbol));
    trace.debug();

    if (symbol != T_FALSE)
      break;

    if (trace.pull(event) != trace.AVAILABLE)
      break;
  };

  trace.set_cursor(c_eventually); // reset the cursor changes during the
                                  // evaluation of the subformula

  return symbol;
}

/**
 * Always(=)
 *
 * Notes:
 *   - Always (=) is equivalent to Eventually(=) (the evaluation at one point)
 *
 */
template <typename T, typename E, timespan b>
three_valued_type always_equal(T &trace, timespan &t) {

  return eventually_equal<T, E, b>(trace, t);
}

/**
 * Always (<)
 *
 * Notes:
 *   - E::eval_phi1 is evaluated while E::eval_phi2 is discarded.
 *
 */
template <typename T, typename E, timespan b>
three_valued_type always_less(T &trace, timespan &t) {

  class Eval_always_less {
  public:
    static three_valued_type eval_phi1(T &trace, timespan &t) {
      return T_TRUE;
    };
    static three_valued_type eval_phi2(T &trace, timespan &t) {
      return b3_not(E::eval_phi1(trace, t));
    };
  };

  size_t c_always = trace.get_cursor();
  auto sf = until_less<T, Eval_always_less, b>(trace, t);
  trace.set_cursor(c_always); // reset the cursor changes during the evaluation
                              // of the subformula

  return b3_not(sf);
}

/**
 * Since (<)
 */
template <typename T, typename E, timespan b>
three_valued_type since_less(T &trace, timespan &t) {
  auto eval_fold = [](T &trace,
                      timespan &t) -> std::pair<four_valued_type, timespan> {
    // eval_b lambda function
    auto eval_b = [](T &trace, timespan &t,
                     four_valued_type &v) -> four_valued_type {
      // eval_i lambda function
      auto eval_i = [](three_valued_type &b1,
                       three_valued_type &b2) -> four_valued_type {
        return (b2 != T_FALSE) ? b3_to_b4(b2)
                               : ((b1 != T_TRUE) ? b3_to_b4(b1) : FV_SYMBOL);
      };

      size_t c = trace.get_cursor();
      DEBUGV_RMTLD3("$compute phi1\n");
      // compute phi1
      three_valued_type cmpphi1 = E::eval_phi1(trace, t);
      DEBUGV_RMTLD3("@compute phi1.\n");
      trace.set_cursor(c); // reset the cursor changes during the evaluation of
                           // the subformula

      c = trace.get_cursor();
      DEBUGV_RMTLD3("$compute phi2\n");
      // compute phi2
      three_valued_type cmpphi2 = E::eval_phi2(trace, t);
      DEBUGV_RMTLD3("@compute phi2.\n");
      trace.set_cursor(c); // reset the cursor changes during the evaluation of
                           // the subformula

      four_valued_type rs = eval_i(cmpphi1, cmpphi2);

      DEBUGV_RMTLD3("eval_b: phi1=%s phi2=%s\n", out_p(cmpphi1),
                    out_p(cmpphi2));

      if (v == FV_SYMBOL) {
        return rs;
      } else {
        return v;
      }
    };

    timespan c_time = t;

    four_valued_type symbol = FV_SYMBOL;

    typename T::buffer_t::event_t event;

    do {
      DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

      trace.read(event);
      c_time = event.getTime();

      if (c_time == t) /* since semantics is strict and non-matching */
        continue;

      if (t>=b && c_time <= t - b)
        break;

      symbol = eval_b(trace, c_time, symbol);

      trace.debug();

      // check if symbol converged and stop!
      if (symbol != FV_SYMBOL)
        break;

    } while (trace.decrement_cursor() == trace.AVAILABLE &&
             trace.read(event) == trace.AVAILABLE);

    return std::make_pair(symbol, c_time);
  };

  size_t c_until = trace.get_cursor();

  DEBUGV_RMTLD3("$(+++) since_op_less\n");

  std::pair<four_valued_type, timespan> eval_c = eval_fold(trace, t);

  DEBUGV_RMTLD3("@(---) since_op (%s) enough(%d)=%d.\n", out_fv(eval_c.first),
                eval_c.second, eval_c.second < t + b);

  trace.set_cursor(c_until); // reset the cursor changes during the evaluation
                             // of the subformula

  return (eval_c.first == FV_SYMBOL)
             ? ((eval_c.second < t - b) ? T_UNKNOWN : T_FALSE)
             : b4_to_b3(eval_c.first);
}

/**
 * PastEventually(=)
 *
 */
template <typename T, typename E, timespan b>
three_valued_type pasteventually_equal(T &trace, timespan &t) {

  timespan c_time = t;
  typename T::buffer_t::event_t event;
  three_valued_type symbol = T_UNKNOWN;

  size_t c_pasteventually = trace.get_cursor();

  do {
    DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

    trace.read(event);
    c_time = event.getTime();

    if (c_time == t) /* since semantics is strict and non-matching */
      continue;

    if (t>=b && c_time <= t - b)
      break;

    size_t c = trace.get_cursor();
    DEBUGV_RMTLD3("$compute phi\n");
    symbol = E::eval_phi1(trace, c_time);
    DEBUGV_RMTLD3("@compute phi.\n");
    trace.set_cursor(
        c); // reset the cursor changes during the evaluation of the subformula

    trace.debug();
  } while (trace.decrement_cursor() == trace.AVAILABLE &&
           trace.read(event) == trace.AVAILABLE);

  trace.set_cursor(c_pasteventually); // reset the cursor changes during the
                                      // evaluation of the subformula

  return symbol;
}

/**
 * Historically(=)
 *
 * Notes:
 *   - Historically (=) is equivalent to PastEventually(=) (the evaluation at
 * one single point)
 *
 */
template <typename T, typename E, timespan b>
three_valued_type historically_equal(T &trace, timespan &t) {

  return pasteventually_equal<T, E, b>(trace, t);
}

/**
 * Historically (<)
 *
 * Notes:
 *   - E::eval_phi1 is evaluated while E::eval_phi2 is discarded.
 *
 */
template <typename T, typename E, timespan b>
three_valued_type historically_less(T &trace, timespan &t) {

  class Eval_historically_less {
  public:
    static three_valued_type eval_phi1(T &trace, timespan &t) {
      return T_TRUE;
    };
    static three_valued_type eval_phi2(T &trace, timespan &t) {
      return b3_not(E::eval_phi1(trace, t));
    };
  };

  size_t c_always = trace.get_cursor();
  auto sf = since_less<T, Eval_historically_less, b>(trace, t);
  trace.set_cursor(c_always); // reset the cursor changes during the evaluation
                              // of the subformula

  return b3_not(sf);
}