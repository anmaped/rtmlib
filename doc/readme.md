[TOC]

Overview {#overview}
========================

The RunTime embedded Monitoring Library (rtmlib) has been initially developed for runtime monitoring of real-time embedded systems either for ARM and X86 platforms. rtmlib is a lean library that supports atomic operations on shared memory circular buffers and implements a monitor abstraction layer for infinite sequences of time-stamped symbols or events. This library is used to implement different monitoring architectures such as the ones proposed in [1] and [2]. Other efficient architectures can be deployed based on lock-free push, pull, pop primitives over inifnite trace sequences containing time-stamped events. The synchronization primitives for push, pull and pop operations allow different readers and writers to progress asynchronously over the instantiated circular buffers and to synchronize when required. Indeed, the rtmlib solves the lock-free producer-consumer problem for circular buffer-based FIFO queues where readers are consumers and writers are producers.

The rtmlibv0.2 is not just an improved version of rtmlibv0.1 but is also a library that supports hardware synthesis via Vivado HLS tool. The rtmlib can support software and hardware monitoring via dedicated CPU and FPGA devices. The figure Hybrid Overview show our new approach.

\image html hybrid_overview.png

rtmlib has a direct connection with the rmtld3synth tool as a monitor integration layer. rmtld3synth is a tool that can generate cpp11 monitors and can be implemented in software or hardware.

Usage of RTMLib {#usagertml}
========================

Interface header and buffer {#istantiating}
-----------------------------

A buffer is a continuous memory array that is shared between the monitor and the system under observation (SUO). The buffer includes sequences with time-stamped symbols or events that identify changes in the SUO state. RTMLib requires at least one global buffer available for system instrumentation, and the link step must provide the buffer address to external monitors. For that, we use a "interface.h" header file as the interface definition to be used by both tasks, SUO and monitor. Note that, by default, a monitor runs on a task. __start_periodic_monitors function is able to start all monitors. Note also that symbols are integers from 0 to 255 by default (uint8_t ). Other types such as uint16_t and uint32_t are also available. Strings and classes should be avoided. We encourage the programmers to statically map these events using a hash table to reduce memory footprint.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
#ifndef INTERFACE_H
#define INTERFACE_H

#include "circularbuffer.h"

extern void __start_periodic_monitors();

// defines a buffer with 100 elements and each element of type uint8_t
extern RTML_buffer<Event<uint8_t>, 100> __buffer;

// symbols that mean events or propositions
#define EV_C 3
#define EV_A 2
#define EV_B 1

#endif // INTERFACE_H
~~~~~~~~~~~~~~~~~~~~~

The instantiation of the buffers and initialization of the monitors should be similar to the next block of code. Note that it is not necessary to instantiate the buffer with the monitors. The buffer is a memory position that must be accessible by the monitor and SUO. Whoever initializes the buffer is left to the programmer.

~~~~~~~~~~~~~~~~~~~~~{.cpp}

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
~~~~~~~~~~~~~~~~~~~~~
`M_simple.h` header defines a monitor according to the section 'Develop a simple monitor' as defined below. `M_morecomplex.h` header defines another monitor that shares the buffer `__buffer_monitor_set1`. `__start_periodic_monitors` is a procedure to trigger the initialization of the monitors.


Develop a simple Monitor {#smonitor}
-----------------------------

Let us build a simple monitor on top of the RTML_monitor class. The RTML_monitor class enables programmers to run monitors over a given period of time, and is started using some arguments such as period, scheduler policy, and priority. The policies and priorities of the scheduler are OS dependent. For instance, we only have the SCHED_FIFO policy available in pthreads-win32, and priorities can be negative ranging from -15(lowest) to 15(highest). Zero is the normal priority. For fully Posix compliant OS, priorities are not negative and there are some other policies such as SCHED_RRR (round robin) and SCHED_OTHER. Since NuttX OS is a Posix compliant OS, we have the same policies.
The next block of code displays the string *Body of the monitor.* several times with a period of `p` useconds.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include <cassert>

#include "interface.h"

#include "periodicmonitor.h"
#include "reader.h"

class M_simple : public RTML_monitor {
private:
  RTML_reader<RTML_buffer<Event<uint8_t>, 100>> __reader =
      RTML_reader<RTML_buffer<Event<uint8_t>, 100>>(__buffer);

  uint8_t gap;

protected:
  void run() {
    static int count = 0;
    ::printf("run simplemonitor job #%d begin...\n", count);

    ::printf("run simplemonitor job #%d end.\n", count);
    count++;
  }

public:
  M_simple(useconds_t p)
      : RTML_monitor(p, SCHED_OTHER, 0), gap(0) {
  } // use SCHED_FIFO for real-time
};
~~~~~~~~~~~~~~~~~~~~~

Let us now overwrite the `run` procedure with a consumer procedure as exemplified in section 'Consumer procedure'.

### Consumer procedure {#consumerp}

The consumer process is exemplified using one lambda function that returns a pointer of type `void *` and receives an argument of type `void *`. It fits the required interface defined in RTML_monitor for the procedure `run`. The body of the function initializes an object of type `RTML_reader<int>` that will be used as the consumer for the lock-free buffer. The procedure `dequeue()` peek a tuple containing an event of type `Event<int>`, where the template typename is the type of the expected identifier of the event, and a time-stamp. Note that the dequeue is local to the reader, does not affect the global buffer, and can be synchronized using a certain time-stamp. However, to get a global dequeue of a certain event, we shall share the same reader among the tasks.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
void run() {
  static int count = 0;
  ::printf("run simplemonitor job #%d begin...\n", count);

  auto consumer = [&](void *) -> void * {

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
~~~~~~~~~~~~~~~~~~~~~

The variable `tmpEvent` stores the dequeued event, where the methods `getTime()` and `getData()` return the time-stamp and the event identifier, respectively.


## Develop a simple instrumentation (Producer procedure) {#producerp}

Let us build a procedure for the production of time-stamped symbols on circular buffers. First, we initialize the `__writer` object of the type `RTML_writer<int>`. Next, we push a symbol of the type `int` with a timestamp to the buffer that accepts events of the type `Event<int>`, and finally we print the buffer with the `debug` helper function to the stdout for debugging purposes.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
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

static __task producer_A = __task("producer", producer, 0, SCHED_OTHER, 2000000); // use SCHED_FIFO for real-time
~~~~~~~~~~~~~~~~~~~~~

Note that `__task` is also an helper function that we use to construct the data descriptor of a task. It is composed by a function pointer, a priority, a scheduler policy, and a period. `100000` means 1/10 seconds.

## Implementation details

Use the example folder to see some simple instrumentation and monitor construction examples.

See [task helpers](task_helpers.md) for more details.

See [lock-free circular buffer](lock_free.md) for more details.

See [the class diagram](other.md) for more information.


## References

[1] de Matos Pedro A., Pereira D., Pinho L.M., Pinto J.S. (2014) Towards a Runtime Verification Framework for the Ada Programming Language. In: George L., Vardanega T. (eds) Reliable Software Technologies – Ada-Europe 2014. Ada-Europe 2014. Lecture Notes in Computer Science, vol 8454. Springer, Cham. https://doi.org/10.1007/978-3-319-08311-7_6

[2] Nelissen G., Pereira D., Pinho L.M. (2015) A Novel Run-Time Monitoring Architecture for Safe and Efficient Inline Monitoring. In: de la Puente J., Vardanega T. (eds) Reliable Software Technologies – Ada-Europe 2015. Ada-Europe 2015. Lecture Notes in Computer Science, vol 9111. Springer, Cham. https://doi.org/10.1007/978-3-319-19584-1_5

