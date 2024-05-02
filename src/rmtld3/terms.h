/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2018-2023 André Pedro
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
 * This header file contain the template to construct the RMTLD3 terms.
 */

/* Duration term */
template <typename T, typename E, timespan t_upper>
duration duration_term(T &trace, timespan &t) {

  typename T::buffer_t::event_t event;
  duration acc = make_duration(0, false);

  /* indicator function */
  auto indicator_function = [](T &trace, timespan &t) -> duration {
    size_t c = trace.get_cursor();
    auto formula = E::eval_phi1(trace, t);
    trace.set_cursor(c); // reset the cursor changes during the evaluation of the subformula

    return (formula == T_TRUE)
               ? make_duration(1, false)
               : ((formula == T_FALSE) ? make_duration(0, false)
                                       : make_duration(0, true));
  };

  size_t c_duration = trace.get_cursor();

  // initialization of c_time_prev
  trace.read(event);
  auto c_time_prev = event.getTime();
  auto symbol = indicator_function(trace, c_time_prev);

  while (trace.pull(event) == trace.AVAILABLE) {

    c_time_prev = event.getTime();

    trace.read(event);
    auto c_time = event.getTime();

    // find lower condition
    if (c_time_prev <= t && t < c_time) {
      acc.first = (c_time - t) * symbol.first;
      acc.second = symbol.second;

      DEBUGV_RMTLD3("dur lower(%ld)\n", c_time_prev);
    }
    // find upper condition
    else if (c_time_prev <= t + t_upper && t + t_upper < c_time) {
      acc.first += (t + t_upper - c_time_prev) * symbol.first;
      acc.second |= symbol.second;

      DEBUGV_RMTLD3("dur upper(%ld)\n", c_time);

      DEBUGV_RMTLD3("duration=%ld unknown=%d (%d,%d)\n", acc.first, acc.second,
                    c_time_prev, c_time);
      break;
    }
    // in between
    else {

      acc.first += (c_time - c_time_prev) * symbol.first;
      acc.second |= symbol.second;
    }

    symbol = indicator_function(trace, c_time);

    DEBUGV_RMTLD3("duration=%ld unknown=%d (%d,%d)\n", acc.first, acc.second,
                  c_time_prev, c_time);
  }

  // we may have more symbols to see (no conclusion)
  if (event.getTime() <= t + t_upper) // [TODO: < or <= ?]
    acc.second = true;

  trace.set_cursor(c_duration); // reset the cursor changes during the evaluation of the subformula

  return acc;
}