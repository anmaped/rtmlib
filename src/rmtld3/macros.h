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
    DEBUGV_RMTLD3("eval: t=%lu prop=%d - (%d %d) next-> (%d %d)\n", t, p,
                  e.getData(), e.getTime(), e_next.getData(), e_next.getTime());
    return e.getData() == p ? T_TRUE : T_FALSE;
  } else {
    DEBUGV_RMTLD3("eval: t=%lu prop=%d - (%d %d) nobound\n", t, p, e.getData(),
                  e.getTime());
    return T_UNKNOWN;
  }
}

/**
 * Until (<)
 */
template <typename T, typename E, timespan b>
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

      DEBUGV_RMTLD3("$compute phi1\n");
      // compute phi1
      three_valued_type cmpphi1 = E::eval_phi1(trace, t);
      trace.set(t); // reset all reads of the subformula
      DEBUGV_RMTLD3("@compute phi1.\n");

      DEBUGV_RMTLD3("$compute phi2\n");
      // compute phi2
      three_valued_type cmpphi2 = E::eval_phi2(trace, t);
      trace.set(t); // reset all reads of the subformula
      DEBUGV_RMTLD3("@compute phi2.\n");

      four_valued_type rs = eval_i(cmpphi1, cmpphi2);

      DEBUGV_RMTLD3("eval_b: phi1=%s phi2=%s\n", out_p(cmpphi1),
                    out_p(cmpphi2));

      if (v == FV_SYMBOL) {
        return rs;
      } else {
        return v;
      }
    };

    trace.set(t); // force start at t

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

      // check if symbol converged and stop!
      if (symbol != FV_SYMBOL)
        break;
    
    } while (trace.pull(event) == trace.AVAILABLE);

    return std::make_pair(symbol, c_time);
  };

  DEBUGV_RMTLD3("$(+++) until_op_less\n");

  std::pair<four_valued_type, timespan> eval_c = eval_fold(trace, t);

  DEBUGV_RMTLD3("@(---) until_op (%s) enough(%d)=%d.\n", out_fv(eval_c.first),
                eval_c.second, eval_c.second < t + b);

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

  trace.set(t); // force start at t

  timespan c_time = t;
  typename T::buffer_t::event_t event;
  three_valued_type symbol = T_UNKNOWN;
  
  do {
      DEBUGV_RMTLD3("t=%d c_time=%d len=%d\n", t, c_time, trace.length());

      trace.read(event);
      c_time = event.getTime();

      if (c_time > b + t)
        break;

      symbol = E::eval_phi1(trace, c_time);
      trace.set(c_time); // force start at c_time

      trace.debug();
  } while (trace.pull(event) == trace.AVAILABLE);

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
 * Eventually (<)
 *
 * Notes:
 *   - E::eval_phi1 is evaluated while E::eval_phi2 is discarded.
 *
 */
template <typename T, typename E, timespan b>
three_valued_type eventually_less(T &trace, timespan &t) {

  class Eval_eventually_less {
  public:
    static three_valued_type eval_phi1(T &trace, timespan &t) {
      return T_TRUE;
    };
    static three_valued_type eval_phi2(T &trace, timespan &t) {
      return E::eval_phi1(trace, t);
    };
  };

  return until_less<T, Eval_eventually_less, b>(trace, t);
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

  auto sf = until_less<T, Eval_always_less, b>(trace, t);
  return b3_not(sf);
}
