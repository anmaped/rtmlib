/* This file was automatically generated from rmtld3synth tool version
v0.4 (184fbccc232e66bfac40de73db696ef28bd99535)
x86_64 GNU/Linux 2024-05-03 18:46. */

#ifndef RTM_MONITOR_6EA3_H
#define RTM_MONITOR_6EA3_H

#include "Rtm_compute_6ea3.h"
#include <periodicmonitor.h>
#include <reader.h>
#include <rmtld3/reader.h>

/* 100, Event< _auto_gen_prop > */
#define RTML_BUFFER0_SIZE 100
#define RTML_BUFFER0_TYPE Event<proposition>
#define RTML_BUFFER0_SETUP()                                                   \
  RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE> __buffer_rtm_monitor_6ea3;

#define RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS()                               \
                                                                               \
  RMTLD3_reader<                                                               \
      RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>>          \
      __trace_rtm_monitor_6ea3_0 = RMTLD3_reader<                              \
          RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>>(     \
          __buffer_rtm_monitor_6ea3);                                          \
                                                                               \
  Rtm_monitor_6ea3_0<RMTLD3_reader<                                            \
      RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>>>         \
      rtm_mon0(200000, __trace_rtm_monitor_6ea3_0);

template <class T>
class Rtm_monitor_6ea3_0 : public RTML_monitor<'6', 'e', 'a', '3'> {

private:
  T &trace;

protected:
  three_valued_type _out = T_UNKNOWN;
  timespan tzero;

  void run() {

    trace.synchronize();

    if (trace.set(tzero) == 0) {
      _out = _rtm_compute_6ea3_0<T>(trace, tzero);
    }

    DEBUG_RMTLD3("- %lu - Status:%d\n", tzero, _out);
  }

public:
  Rtm_monitor_6ea3_0(useconds_t p, T &trc)
      : RTML_monitor(p, DEFAULT_SCHED, 50), trace(trc) {
    tzero = clockgettime() + 3000000000L;
  }

  Rtm_monitor_6ea3_0(useconds_t p, T &trc, int sche, int prio)
      : RTML_monitor(p, sche, prio), trace(trc) {
    tzero = clockgettime() + 3000000000L;
  }
  three_valued_type &getVeredict() { return _out; }
};
#endif // RTM_MONITOR_6EA3_H
