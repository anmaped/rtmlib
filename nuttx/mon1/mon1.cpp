#include "Mon0.h"
#include "RTML_buffer.h"

int count_until_iterations;

#ifdef __NUTTX__
__EXPORT RTML_buffer<int, 100> __buffer_mon1 __attribute__((used));
#else
RTML_buffer<int, 100> __buffer_mon1 __attribute__((used));
#endif
Mon0 mon_mon0(0);

void __start_periodic_monitors()
{
  if (mon_mon0.enable()) {::printf("ERROR\n");}

}

