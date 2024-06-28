/* This file was automatically generated from rmtld3synth tool version
  v0.4-5-g4f6dc2b (4f6dc2bab216608513653509357ed5d4f99f6546)
x86_64 GNU/Linux 2024-06-25 11:24. */

#ifndef RTM_INSTRUMENT_62BE_H_
#define RTM_INSTRUMENT_62BE_H_

#include <rmtld3/rmtld3.h>
#include <writer.h>

#define RTM_INSTRUMENT_62BE_BUFFER_SIZE 100

class RTM_INSTRUMENT_62BE
{
public:
  typedef enum _auto_gen_prop
  {
    a = 2,
    b = 1,
  } prop_t;

  typedef Event<std::underlying_type<_auto_gen_prop>::type> buffer_t;
};

template <typename T, T &buffer>
class Writer_RTM_INSTRUMENT_62BE : public RTM_INSTRUMENT_62BE
{

public:
  typename T::error_t
  push (std::underlying_type<_auto_gen_prop>::type s)
  {
    typename T::event_t e = typename T::event_t (s, 0);
    return w.push (e);
  };

  typename T::error_t
  push (std::underlying_type<_auto_gen_prop>::type s, timespan t)
  {
    typename T::event_t e = typename T::event_t (s, t);
    return w.push_all (e);
  };

private:
  RTML_writer<T> w = RTML_writer<T> (buffer);
};

// buffer will be assigned at ld step
extern RTML_buffer<
    Event<std::underlying_type<RTM_INSTRUMENT_62BE::prop_t>::type>,
    RTM_INSTRUMENT_62BE_BUFFER_SIZE>
    __buffer_rtm_monitor_62be;

using Writer_rtm__62be = Writer_RTM_INSTRUMENT_62BE<
    RTML_buffer<Event<std::underlying_type<RTM_INSTRUMENT_62BE::prop_t>::type>,
                RTM_INSTRUMENT_62BE_BUFFER_SIZE>,
    __buffer_rtm_monitor_62be>;

#endif // RTM_INSTRUMENT_62BE_H_

