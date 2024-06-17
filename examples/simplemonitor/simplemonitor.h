#include <cassert>

#include "interface.h"

#include "periodicmonitor.h"
#include "reader.h"

class M_simple : public RTML_monitor<'0'> {
private:
  RTML_reader<RTML_buffer<Event<uint8_t>, 100>> __reader =
      RTML_reader<RTML_buffer<Event<uint8_t>, 100>>(__buffer);

  uint8_t gap;

protected:
  void run() {
    static int count = 0;
    ::printf("run simplemonitor job #%d begin...\n", count);

    auto consumer = [&](void *) -> void * {
      /*static RTML_reader<RTML_buffer<Event<uint8_t>, 100>> __reader =
          RTML_reader<RTML_buffer<Event<uint8_t>, 100>>(__buffer);*/

      if (__reader.synchronize())
        gap++;

      assert(gap <= 1); // ensure that we have no gaps

      Event<uint8_t> tmpEvent;
      auto status = __reader.pull(tmpEvent);

      ::printf("consumed event: %lu, %d status: %d\n", tmpEvent.getTime(),
               tmpEvent.getData(), status);

      __reader.debug();

      return NULL;
    };

    consumer(NULL);

    ::printf("run simplemonitor job #%d end.\n", count);
    count++;
  }

public:
  M_simple(useconds_t p)
      : RTML_monitor(p, SCHED_OTHER, 0), gap(0) {
  } // use SCHED_FIFO for real-time
};
