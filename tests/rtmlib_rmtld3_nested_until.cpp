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

/*
 * rmtlib_rmtld3 with nested always
 */

#include <cstring>

#include <circularbuffer.h>
#include <reader.h>
#include <rmtld3/formulas.h>
#include <rmtld3/reader.h>

template <typename T> class Eval_until_less_1 {
public:
  static three_valued_type eval_phi1(T &trace, timespan &t) { return T_TRUE; };
  static three_valued_type eval_phi2(T &trace, timespan &t) {
    proposition p = 1;
    auto sf = prop<T>(trace, p, t);
    return b3_not(sf);
  };
};

template <typename T> class Eval_until_less_2 {
public:
  static three_valued_type eval_phi1(T &trace, timespan &t) { return T_TRUE; };
  static three_valued_type eval_phi2(T &trace, timespan &t) {
    auto sf = until_less<T, Eval_until_less_1<T>, 10>(trace, t);
    return sf;
  };
};

extern "C" int rtmlib_rmtld3_nested_until();

int rtmlib_rmtld3_nested_until() {

  typedef Event<int> event_t;
  typedef RTML_buffer<event_t, 100> buffer_t;
  typedef RMTLD3_reader<RTML_reader<buffer_t>, int> trace_t;

  buffer_t buf;
  unsigned long int index = 0;

  event_t e[6] = {
      event_t(1, 2),  event_t(1, 5),  event_t(1, 9),
      event_t(1, 14), event_t(1, 19), event_t(2, 20),
  };

  for (int i = 0; i < 6; i++)
    buf.push(e[i]);

  // check first element of the trace
  {
    event_t event;
    buf.read(event, index);
    assert(event.getTime() == 2 && event.getData() == 1);
  };

  int tzero = 0.;
  trace_t trace = trace_t(buf, tzero);

  trace.synchronize();

  auto _mon0_compute = [](trace_t &trace, timespan &t) -> three_valued_type {
    auto sf = until_less<trace_t, Eval_until_less_2<trace_t>, 10>(trace, t);
    return b3_not(sf);
  };

  timespan t = 2;
  auto _out = _mon0_compute(trace, t);

  if (strcmp(out_p(_out), "true") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}
