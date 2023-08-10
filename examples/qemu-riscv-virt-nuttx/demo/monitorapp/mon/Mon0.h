
  #ifndef MONITOR_MON0_H
  #define MONITOR_MON0_H

  #include "reader.h"
  #include "periodicmonitor.h"
  #include "rmtld3/reader.h"
  #include "mon0_compute.h"

  /* 100, Event< proposition > */
  #define RTML_BUFFER0_SIZE 100
  #define RTML_BUFFER0_TYPE Event< proposition >
  #define RTML_BUFFER0_SETUP()                                                   \
    RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE> __buffer0;                 \
    int tzero = 0;
  
  #define RTML_BUFFER0_TRIGGER_PERIODIC_MONITORS()                                           \
    RMTLD3_reader<                                                               \
        RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int>     \
        trace = RMTLD3_reader<                                                   \
            RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>,      \
            int>(__buffer0, tzero);                                              \
    Mon0<RMTLD3_reader<                                                          \
        RTML_reader<RTML_buffer<RTML_BUFFER0_TYPE, RTML_BUFFER0_SIZE>>, int>>    \
        mon_mon0(100000, trace);

  template<class T>
  class Mon0 : public RTML_monitor {

  private:
    T &trace;

  protected:
    void run(){

      timespan tzero = 0;
      three_valued_type _out = _mon0_compute<T>(trace,tzero);
      DEBUG_RTMLD3("Status:%d\n", _out);
    }

  public:
    Mon0(useconds_t p, T& trc): RTML_monitor(p,SCHED_FIFO,50), trace(trc) {}
    Mon0(useconds_t p, T& trc, int sche, int prio): RTML_monitor(p,sche,prio), trace(trc) {}

  };

  #endif //MONITOR_MON0_H
