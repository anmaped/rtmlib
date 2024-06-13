/* This file was automatically generated from rmtld3synth tool version
  v0.4 (184fbccc232e66bfac40de73db696ef28bd99535)
x86_64 GNU/Linux 2024-05-03 18:46. */

#ifndef RTM_INSTRUMENT_8EA7_H_
#define RTM_INSTRUMENT_8EA7_H_

#include <rmtld3/rmtld3.h>
#include <writer.h>

template <typename T, T &buffer> class Writer_RTM_INSTRUMENT_8EA7 {

public:
  enum _auto_gen_prop {
    a = 2,
    b = 1,
  };

  typedef Event<_auto_gen_prop> buffer_t;

  typename T::error_t push(_auto_gen_prop s, timespan t) {
    typename T::event_t e = typename T::event_t(s, t);
    return w.push(e);
  };

private:
  RTML_writer<T> w = RTML_writer<T>(buffer);
};

// buffer will be assigned at ld step
extern RTML_buffer<Event<proposition>, 100> __buffer_rtm_monitor_8ea7;

using Writer_rtm__8ea7 =
    Writer_RTM_INSTRUMENT_8EA7<RTML_buffer<Event<proposition>, 100>,
                               __buffer_rtm_monitor_8ea7>;

#endif // RTM_INSTRUMENT_8EA7_H_
