/* This file was automatically generated from rmtld3synth tool version
v0.3-alpha2-35-g8a06be4 (8a06be4993438cc33c2c7e07d57523efb74088a8)
x86_64 GNU/Linux 2023-08-11 23:46. */

#ifndef RTM_MONITOR_0EE2_H
#define RTM_MONITOR_0EE2_H

#include "Rtm_compute_0ee2.h"
#include "periodicmonitor.h"
#include "reader.h"
#include "rmtld3/reader.h"

/* 100, Event< proposition > */
#define RTML_BUFFER0_SIZE 100
#define RTML_BUFFER0_TYPE Event<proposition>
#define RTML_BUFFER0_SETUP()                                                   \
  RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE> __buffer_rtm_monitor_0ee2; \
  int tzero = 0;

#define RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS()                               \
                                                                               \
  RMTLD3_reader<                                                               \
      RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int>     \
      __trace_rtm_monitor_0ee2_0 = RMTLD3_reader<                              \
          RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>,      \
          int>(__buffer_rtm_monitor_0ee2, tzero);                              \
                                                                               \
  Rtm_monitor_0ee2_0<RMTLD3_reader<                                            \
      RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int>>    \
      rtm_mon0(10000, __trace_rtm_monitor_0ee2_0);

template <class T> class Rtm_monitor_0ee2_0 : public RTML_monitor<'0'> {

private:
  T &trace;

protected:
  void run() {

    timespan tzero = 0;
    three_valued_type _out = _rtm_compute_0ee2_0<T>(trace, tzero);
    DEBUG_RMTLD3("Status:%d\n", _out);
  }

public:
  Rtm_monitor_0ee2_0(useconds_t p, T &trc)
      : RTML_monitor(p, SCHED_FIFO, 50), trace(trc) {}
  Rtm_monitor_0ee2_0(useconds_t p, T &trc, int sche, int prio)
      : RTML_monitor(p, sche, prio), trace(trc) {}
};

#endif // RTM_MONITOR_0EE2_H

