
#include <unistd.h>

#include "interface.h"
#include "task_compat.h"
#include "writer.h"

// system under observation
void system_under_observation() {

  auto producer = [](void *) -> void * {
    static RTML_writer<RTML_buffer<Event<uint8_t>, 100>> __writer =
        RTML_writer<RTML_buffer<Event<uint8_t>, 100>>(__buffer);

    static int count = 0;
    ::printf("run suo job #%d begin...\n", count);

    Event<uint8_t> id(EV_A, 0); // set event to id EV_A
    __writer.push(id);       // writer will time-stamp the event

    __buffer.debug();

    ::printf("run suo job #%d end.\n", count);
    count++;

    return NULL;
  };

  static __task producer_A = __task("suo", producer, 0, SCHED_OTHER, 2000000); // use SCHED_FIFO for real-time
}

// suo and monitors
int main() {
  system_under_observation();
  __start_periodic_monitors();

  ::printf("suo is running ...\n");
  while (true) {
    sleep(1);
  };
}
