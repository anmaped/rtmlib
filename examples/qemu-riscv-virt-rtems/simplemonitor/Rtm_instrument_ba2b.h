/* This file was automatically generated from rmtld3synth tool version
  v0.4-alpha-16-g4d5a726 (4d5a726f8005c6765b1871d3cd7638b7caffadcb)
x86_64 GNU/Linux 2024-04-24 23:36. */
  
  #ifndef RTM_INSTRUMENT_BA2B_H_
  #define RTM_INSTRUMENT_BA2B_H_


      #include <writer.h>
  #include <rmtld3/rmtld3.h>

template<typename T, T& buffer>
class Writer_RTM_INSTRUMENT_BA2B {

  public:enum prop {a = 2, b = 1, };

    typedef Event< prop > buffer_t;

    typename T::error_t push(prop s, timespan t) {
      typename T::event_t e = typename T::event_t(s,t);
      return w.push(e);
    };

  private:
    RTML_writer<T> w = RTML_writer<T>(buffer);

};

// buffer will be assigned at ld step
extern RTML_buffer<Event<proposition>, 100> __buffer_rtm_monitor_ba2b;

using Writer_rtm__ba2b = Writer_RTM_INSTRUMENT_BA2B<RTML_buffer<Event<proposition>, 100>,__buffer_rtm_monitor_ba2b>;

     #endif //RTM_INSTRUMENT_BA2B_H_
