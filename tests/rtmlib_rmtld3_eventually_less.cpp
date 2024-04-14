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

/*
 * rmtlib_rmtld3 with eventually
 */

#include <cstring>

#include <circularbuffer.h>
#include <reader.h>
#include <rmtld3/formulas.h>
#include <rmtld3/reader.h>

template <typename T> class Eval_eventually {
public:
  static three_valued_type eval_phi1(T &trace, timespan &t) {
    proposition p = 2;
    auto sf = prop<T>(trace, p, t);
    return sf;
  };
  static three_valued_type eval_phi2(T &trace, timespan &t) {
    return T_UNKNOWN;
  };
};

extern "C" int rtmlib_rmtld3_eventually_less();

int rtmlib_rmtld3_eventually_less() {

  typedef Event<int> event_t;
  typedef RTML_buffer<event_t, 100> buffer_t;
  typedef RMTLD3_reader<RTML_reader<buffer_t>, int> trace_t;

  buffer_t buf;
  unsigned long int index = 0;

  event_t e[7] = {
      event_t(1, 2),  event_t(3, 5),  event_t(1, 9),  event_t(3, 14),
      event_t(1, 19), event_t(2, 21), event_t(3, 25),
  };

  for (int i = 0; i < 7; i++)
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
    auto sf = eventually_less<trace_t, Eval_eventually<trace_t>, 22>(trace, t);
    return sf;
  };

  timespan t = 2;
  auto _out = _mon0_compute(trace, t);

  if (strcmp(out_p(_out), "true") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}
