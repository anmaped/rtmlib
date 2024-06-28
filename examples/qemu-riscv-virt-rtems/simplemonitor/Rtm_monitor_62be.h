/* This file was automatically generated from rmtld3synth tool version
v0.4-5-g4f6dc2b (4f6dc2bab216608513653509357ed5d4f99f6546)
x86_64 GNU/Linux 2024-06-25 11:24. */

#ifndef RTM_MONITOR_62BE_H
#define RTM_MONITOR_62BE_H

#include "Rtm_compute_62be.h"
#include <periodicmonitor.h>
#include <reader.h>
#include <rmtld3/reader.h>

/* 100, Event< std::underlying_type<_auto_gen_prop>::type > */
#define RTM_MONITOR_62BE_BUFFER_SIZE 100
#define RTM_MONITOR_62BE_BUFFER_TYPE                                          \
  Event<std::underlying_type<_auto_gen_prop>::type>
#define RTML_BUFFER0_SETUP()                                                  \
  RTML_buffer<RTM_MONITOR_62BE_BUFFER_TYPE, RTM_MONITOR_62BE_BUFFER_SIZE>     \
      __buffer_rtm_monitor_62be;
#define RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS()                              \
  RMTLD3_reader<RTML_reader<RTML_buffer<RTM_MONITOR_62BE_BUFFER_TYPE,         \
                                        RTM_MONITOR_62BE_BUFFER_SIZE> > >     \
      __trace_rtm_monitor_62be_0 = RMTLD3_reader<RTML_reader<RTML_buffer<     \
          RTM_MONITOR_62BE_BUFFER_TYPE, RTM_MONITOR_62BE_BUFFER_SIZE> > > (   \
          __buffer_rtm_monitor_62be);                                         \
                                                                              \
  Rtm_monitor_62be_0<RMTLD3_reader<RTML_reader<RTML_buffer<                   \
      RTM_MONITOR_62BE_BUFFER_TYPE, RTM_MONITOR_62BE_BUFFER_SIZE> > > >       \
      rtm_mon0 (200000, __trace_rtm_monitor_62be_0);

template <class T>
class Rtm_monitor_62be_0 : public RTML_monitor<'6', '2', 'b', 'e'>
{

private:
  T &trace;

protected:
  three_valued_type _out = T_UNKNOWN;
  timespan tbegin;

  void
  run ()
  {

    trace.synchronize ();
    if (trace.set (tbegin) == 0)
      {
        _out = _rtm_compute_62be_0<T> (trace, tbegin);
      }
    DEBUG_RMTLD3 ("status=%d, tbegin=%lu\n", _out, tbegin);
  }

public:
  Rtm_monitor_62be_0 (useconds_t p, T &trc, timespan &t)
      : RTML_monitor (p, DEFAULT_SCHED, DEFAULT_PRIORITY), trace (trc),
        tbegin (t)
  {
  }
  Rtm_monitor_62be_0 (useconds_t p, T &trc)
      : RTML_monitor (p, DEFAULT_SCHED, DEFAULT_PRIORITY), trace (trc)
  {
    tbegin = clockgettime ()  + 3000000000L;
  }
  Rtm_monitor_62be_0 (useconds_t p, T &trc, int sche, int prio)
      : RTML_monitor (p, sche, prio), trace (trc)
  {
    tbegin = clockgettime ()  + 3000000000L;
  }
  Rtm_monitor_62be_0 (useconds_t p, T &trc, int sche, int prio, timespan &t)
      : RTML_monitor (p, sche, prio), trace (trc), tbegin (t)
  {
  }
  three_valued_type &
  getVeredict ()
  {
    return _out;
  }
};

#endif // RTM_MONITOR_62BE_H

