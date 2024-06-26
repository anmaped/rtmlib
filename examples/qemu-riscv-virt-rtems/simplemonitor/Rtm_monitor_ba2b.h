/* This file was automatically generated from rmtld3synth tool version
v0.4-alpha-16-g4d5a726 (4d5a726f8005c6765b1871d3cd7638b7caffadcb)
x86_64 GNU/Linux 2024-04-24 23:36. */

  #ifndef RTM_MONITOR_BA2B_H
  #define RTM_MONITOR_BA2B_H

  #include <reader.h>
  #include <periodicmonitor.h>
  #include <rmtld3/reader.h>
  #include "Rtm_compute_ba2b.h"

  /* 100, Event< prop > */
  #define RTML_BUFFER0_SIZE 100
  #define RTML_BUFFER0_TYPE Event< proposition >
  #define RTML_BUFFER0_SETUP() \
    RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE> __buffer_rtm_monitor_ba2b;\
    int tzero = 0;

    #define RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS() \
\
RMTLD3_reader< \
RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int> \
__trace_rtm_monitor_ba2b_0 = RMTLD3_reader< \
    RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>,\
    int>(__buffer_rtm_monitor_ba2b, tzero); \
\
Rtm_monitor_ba2b_0<RMTLD3_reader< \
        RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int>> \
        rtm_mon0(200000, __trace_rtm_monitor_ba2b_0); \


  template<class T>
  class Rtm_monitor_ba2b_0 : public RTML_monitor {

  private:
    T &trace;

  protected:
    void run(){

      timespan tzero = 0;
      three_valued_type _out = _rtm_compute_ba2b_0<T>(trace,tzero);
      DEBUG_RMTLD3("Status:%d\n", _out);
    }

  public:
    Rtm_monitor_ba2b_0(useconds_t p, T& trc): RTML_monitor(p,SCHED_FIFO,50), trace(trc) {}
    Rtm_monitor_ba2b_0(useconds_t p, T& trc, int sche, int prio): RTML_monitor(p,sche,prio), trace(trc) {}

  };

  #endif //RTM_MONITOR_BA2B_H
