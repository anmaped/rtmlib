
#include "interface.h"     // the interface to use with SUO
#include "simplemonitor.h" // includes the monitor 1 as M_simple class

// constructs a global __buffer buffer
RTML_buffer<Event<uint8_t>, 100> __buffer;

// construct two simple monitors
M_simple simplem1(5000000);  // 5 seconds period
M_simple simplem2(10000000); // 10 seconds period

// enables the monitor initialization
void __start_periodic_monitors() {
  if (simplem1.enable()) {
    ::printf("ERROR\n");
  }
  if (simplem2.enable()) {
    ::printf("ERROR\n");
  }
}
