/**
 * rmtlib_rmtld3 with nested always
 */

#include <cstring>

#include <RTML_buffer.h>
#include <RTML_reader.h>
#include <rmtld3/reader.h>
#include <rmtld3/rmtld3.h>
#include <rmtld3/macros.h>

template <typename T> class Eval_until_less_1 {
public:
  static three_valued_type eval_phi1(T &trace, timespan& t) { return T_TRUE; };
  static three_valued_type eval_phi2(T &trace, timespan& t) {
    proposition p = 1;
    auto sf = prop<T>(trace, p, t);
    return b3_not(sf);
  };
};

template <typename T> class Eval_until_less_2 {
public:
  static three_valued_type eval_phi1(T &trace, timespan& t) { return T_TRUE; };
  static three_valued_type eval_phi2(T &trace, timespan& t) {
    auto sf = until_less<T, Eval_until_less_1<T>, 10>(trace, t);
    return sf;
  };
};


int rtmlib_rmtld3_nested_until() {

  typedef Event<int> event_t;
  typedef RTML_buffer<event_t, 100> buffer_t;
  typedef RMTLD3_reader<RTML_reader<buffer_t>,int> trace_t;

  buffer_t buf;
  unsigned long int index = 0;

  event_t e[6] = {event_t(1, 2), event_t(1, 5), event_t(1, 9), event_t(1, 14), event_t(1,19), event_t(2,20), };

  for(int i=0; i<6; i++)
    buf.push(e[i]);

  // check first element of the trace
  { event_t event; buf.read(event, index); assert(event.getTime() == 2 && event.getData() == 1); };

  int tzero = 0.;
  trace_t trace = trace_t(buf, tzero);

  trace.synchronize();

  auto _mon0_compute = [](trace_t &trace, timespan& t) -> three_valued_type {
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
