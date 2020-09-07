#ifndef INTERFACE_H
#define INTERFACE_H

#include "circularbuffer.h"

extern void __start_periodic_monitors();

// defines a buffer with 100 elements and each element of type uint8_t
extern RTML_buffer<Event<uint8_t>, 100> __buffer;

// symbols that mean events or propositions
#define EV_C 3
#define EV_A 4
#define EV_set_off 5
#define EV_B 1

#endif // INTERFACE_H
