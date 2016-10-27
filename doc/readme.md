[TOC]

Overview {#overview}
========================

RunTime Embedded Monitoring Library (RTEMLib) has been developed for runtime monitoring of real-time embedded systems. RTEMLib is based on lock-free ring buffer FIFO queues either for ARM and X86 platforms. This library is used to implement different monitoring architectures such as the ones proposed in [[1]](http://link.springer.com/chapter/10.1007%2F978-3-319-19584-1_5) and [[2]](http://link.springer.com/chapter/10.1007%2F978-3-319-08311-7_6). Other efficient architectures can be developed based on lock-free enqueue and dequeue primitives over trace sequences containing time stamped events. Synchronization primitives for dequeue operations allow different readers to progress synchronously over instantiated buffers.

RTEMLib solves the lock-free producer-consumer problem for ring buffer-based FIFO queues where readers are consumers and writers are producers.

Usage of RTEMLib {#usagerteml}
========================

Instantiating buffers {#istantiating}
-----------------------------

Buffers are resources shared among the system under observation (SUO) and monitors. They contain time-stamped event sequences to inform monitors of the SUO state changes. RTEMLib requires at least one global buffer available for the instrumentation of the system, and the linking step shall provide the address of the buffer for external monitors knowing it. We define the "interface.h" as the interface header to be used by both tasks (SUO and monitors).

~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include "RTEML_buffer.h"

extern void __start_periodic_monitors();

// defining one buffer with size 100 of type uint8_t
extern RTEML_buffer<uint8_t, 100> __buffer_monitor_set1;

#define EV_C 3
#define EV_A 4
#define EV_set_off 5
#define EV_B 1
~~~~~~~~~~~~~~~~~~~~~
Note that uint8_t could be used to represent events from 0 to 255 only. Other types such as uint16_t and uint32_t are available as well. Strings and classes are discouraged by us to be part of events. We encourage the programmers to map these events in one hash table to save memory usage.

The instantiation of buffers and monitors together shall be something like the code next. Note that is not mandatory to instantiate the buffer with the monitors, indicating that buffers could be instantiated with the SUO.
~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include "M_morecomplex.h"
#include "M_simple.h"
#include "RTEML_buffer.h"

RTEML_buffer<uint8_t, 100> __buffer_monitor_set1;

M_morecomplex mon_m_morecomplex(__buffer_monitor_set1, 500000);
M_simple mon_m_simple(__buffer_monitor_set1, 1000000);

void __start_periodic_monitors()
{
  if (mon_m_morecomplex.enable()) {::printf("ERROR\n");}
  if (mon_m_simple.enable()) {::printf("ERROR\n");}
}
~~~~~~~~~~~~~~~~~~~~~
`M_simple.h` header defines a monitor according to the section 'Developing a simple monitor' as defined below. `M_morecomplex.h` header defines another monitor that shares the buffer `__buffer_monitor_set1`. `__start_periodic_monitors` is a proceeder to initialize both monitors when is required by the monitoring thread.


Developing a simple Monitor {#smonitor}
-----------------------------
Lets construct a simple monitor based on RTEML_monitor class. First, RTEML_monitor class enables monitors to execute at a certain periodicity. The class is initialized using some arguments such as the period, the scheduler policy, and the priority. The scheduler policies and priorities are commonly OS dependent. For instance, in Windows Embedded 10 x86, we only have available the SCHED_FIFO policy in pthreads-win32, and priorities can be negative ranging from -15(lowest) to 15(highest). Zero is the normal priority.

For fully Posix compliant OS, the priorities are non negative and several policies such as SCHED_RR (round robin) and SCHED_OTHER exist. In case of NuttX OS, we have the same policies.
~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include "interface.h"

class M_simple : public RTEML_monitor {

  private:
    RTEML_reader<int> __reader = RTEML_reader<int>(__buffer_monitor_set1.getBuffer());

  protected:
    void run(){
      ::printf("Body of the monitor.");
    }

  public:
    M_simple(useconds_t p): RTEML_monitor(p,SCHED_FIFO,5) {}

};
~~~~~~~~~~~~~~~~~~~~~
This monitor will display the string *Body of the monitor.* several times with a period of `p` useconds. Lets replace the `run` proceeder with a consumer procedure as exemplified in section 'Consumer procedure' below.

### Consumer procedure {#consumerp}

The consumer process is exemplified using one lambda function that returns a pointer of type `void *` and receives an argument of type `void *`. It fits the required interface defined in RTEML_monitor for the procedure `run`. The body of the function initializes an object of type `RTEML_reader<int>` that will be used as the consumer for the lock-free buffer. The procedure `dequeue()` peek a tuple containing an event of type `Event<int>`, where the template typename is the type of the expected identifier of the event, and a time-stamp. Note that the dequeue is local to the reader, does not affect the global buffer, and can be synchronized using a certain time-stamp. However, to get a global dequeue of a certain event, we shall share the same reader among the tasks.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
auto consumer = [](void *) -> void*
{
	static RTEML_reader<int> __reader = RTEML_reader<int>(__buffer_monitor_set1.getBuffer());
	Event<int> tmpEvent;

	std::pair<state_rd_t,Event<int> &> rd_tuple = __reader.dequeue();
	tmpEvent = rd_tuple.second;
	::printf("event_out: %lu, %d code: %d\n", tmpEvent.getTime(), tmpEvent.getData(), rd_tuple.first);

	return NULL;
};
~~~~~~~~~~~~~~~~~~~~~

The variable `tmpEvent` stores the dequeued event, where the methods `getTime()` and `getData()` return the time-stamp and the event identifier, respectively.

See [the class diagram](other.md) for more information.

## Producer procedure for Monitors {#producerp}

Lets construct a producer for the lock-free ring buffers. First, we initialize the object `__writer` of the type `RTEML_writer<int>`. Then, we enqueue a value of type `int` to the buffer that accepts events of the type `Event<int>`, and finally print the buffer to the stdout for debugging purposes.

~~~~~~~~~~~~~~~~~~~~~{.cpp}
auto producer = [](void *) -> void*
{
	static RTEML_writer<int> __writer = RTEML_writer<int>(__buffer_monitor_set1.getBuffer());

	__writer.enqueue(1);

	__buffer_monitor_set1.debug();
	return NULL;
};

__task producer_A = __task(producer, 0, SCHED_FIFO, 100000);
~~~~~~~~~~~~~~~~~~~~~

Note that `__task` is an helper used to construct the data descriptor of one task. It inputs the function pointer, the priority, the scheduler policy, and the period. `100000` means 1/10 seconds.

See [task helpers](task_helpers.md) for more details.

See how [lock-free ring buffer](lock_free.md) is implemented in detail.

