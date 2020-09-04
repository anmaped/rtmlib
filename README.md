# rtmlib v0.2

[![Build Status](https://travis-ci.org/anmaped/rtmlib.svg?branch=v0.2)](https://travis-ci.org/anmaped/rtmlib)

## Description

The RunTime embedded Monitoring Library (rtmlib) has been initially developed for runtime monitoring of real-time embedded systems either for ARM and X86 platforms. rtmlib is a lean library that supports atomic operations on shared memory circular buffers and implements a monitor abstraction layer for infinite sequences of time-stamped symbols or events. This library is used to implement different monitoring architectures such as the ones proposed in [1] and [2]. Other efficient architectures can be deployed based on lock-free push, pull, pop primitives over inifnite trace sequences containing time-stamped events. The synchronization primitives for push, pull and pop operations allow different readers and writers to progress asynchronously over the instantiated circular buffers and to synchronize when required. Indeed, the rtmlib solves the lock-free producer-consumer problem for circular buffer-based FIFO queues where readers are consumers and writers are producers.

The rtmlibv0.2 is not just an improved version of rtmlibv0.1 but is also a library that supports hardware synthesis via Vivado HLS tool. The rtmlib can support software and hardware monitoring via dedicated CPU and FPGA devices. The figure Hybrid Overview show our new approach.

![hybrid overview](doc/images/hybrid_overview.png)

rtmlib has a direct connection with the rmtld3synth tool as a monitor integration layer. rmtld3synth is a tool that can generate cpp11 monitors and can be implemented in software or hardware.


## How to use rtmlib?

See [https://anmaped.github.io/rtmlib/doc/](https://anmaped.github.io/rtmlib/doc/) for more details.

### Run rtmlibv0.2 tests

Use the `make` command to build the unit tests.
```
cd tests/
make
```

The expected output is something like
```
$ ./rtmlib_unittests 
rtmlib_rmtld3_nested_until.cpp success.
rtmlib_buffer_push_and_pull.cpp success.
rtmlib_buffer_push_and_pop.cpp success.
rtmlib_reader_and_writer.cpp success.

[...]
$
```


## References

[1] de Matos Pedro A., Pereira D., Pinho L.M., Pinto J.S. (2014) Towards a Runtime Verification Framework for the Ada Programming Language. In: George L., Vardanega T. (eds) Reliable Software Technologies – Ada-Europe 2014. Ada-Europe 2014. Lecture Notes in Computer Science, vol 8454. Springer, Cham. https://doi.org/10.1007/978-3-319-08311-7_6

[2] Nelissen G., Pereira D., Pinho L.M. (2015) A Novel Run-Time Monitoring Architecture for Safe and Efficient Inline Monitoring. In: de la Puente J., Vardanega T. (eds) Reliable Software Technologies – Ada-Europe 2015. Ada-Europe 2015. Lecture Notes in Computer Science, vol 9111. Springer, Cham. https://doi.org/10.1007/978-3-319-19584-1_5



