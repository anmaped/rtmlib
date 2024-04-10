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
 * rmtlib_rmtld3 with always
 */

#include <cstring>

#include <circularbuffer.h>
#include <reader.h>
#include <rmtld3/macros.h>
#include <rmtld3/reader.h>

template <typename T> class Eval_always_b_1_next
{
public:
  static three_valued_type
  eval_phi1 (T &trace, timespan &t)
  {
    auto sf = prop<T> (trace, 2, t);
    return sf;
  };
  static three_valued_type
  eval_phi2 (T &trace, timespan &t)
  {
    return T_UNKNOWN;
  };
};


extern "C" int rtmlib_rmtld3_next_less();

int rtmlib_rmtld3_next_less() {

  typedef Event<int> event_t;
  typedef RTML_buffer<event_t, 100> buffer_t;
  typedef RMTLD3_reader<RTML_reader<buffer_t>, int> trace_t;

  buffer_t buf;
  unsigned long int index = 0;

  event_t e[6] = {
      event_t(1, 2),  event_t(2, 3),  event_t(3, 9),  event_t(3, 10)
  };

  for (int i = 0; i < 6; i++)
    buf.push(e[i]);

  int tzero = 0.;
  trace_t trace = trace_t(buf, tzero);

  trace.synchronize();

  auto _mon0_compute = [](trace_t &trace, timespan &t) -> three_valued_type {
    return [] (trace_t &trace, timespan &t) {
      auto x = [] (trace_t &trace, timespan &t) {
        auto x = prop<trace_t> (trace, 1, t);
        return b3_not (x);
      }(trace, t);
      auto y = always_equal<trace_t, Eval_always_b_1_next<trace_t>, 1> (trace, t);
      return b3_or (x, y);
    }(trace, t);
  };

  timespan t = 2;
  auto _out = _mon0_compute(trace, t);

  //auto _out = until_less<trace_t, Eval_until_less_1_next<trace_t>, 10> (trace, t);

  if (strcmp(out_p(_out), "true") == 0)
    printf("%s \033[0;32msuccess.\e[0m\n", __FILE__);
  else
    printf("%s \033[0;31mFail.\e[0m\n", __FILE__);

  return 0;
}
