/*
 *  rtmlib is a Real-Time Monitoring Library.
 *
 *    Copyright (C) 2020 André Pedro
 *
 *  This file is part of rtmlib.
 *
 *  rtmlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  rtmlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rtmlib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _RTML_BUFFER_H_
#define _RTML_BUFFER_H_

#include <stdio.h>
//#include <time.h>

#include "Event.h"
#include "debug_compat.h"

/**
 * RTML_buffer implements a circular buffer. This buffer is the support
 * for RTML_reader and RTML_writer classes. For instance, The monitor uses
 * an instance of RTML_reader and SUO uses RTML_writer.
 *
 * @see Event
 * @see RTML_reader
 * @see RTML_monitor
 *
 * @author André Pedro
 * @date
 */
template<typename T, size_t N>
class RTML_buffer {
private:
    /**
     * The Event buffer where events are kept. Size is defined via template
     * parameter N which avoids dynamic memory usage.
     *
     * @see Event
     */
    Event<T> array[N];

    /**
     *
     */
    size_t size;

    /*
     * Absolute time epoch.
     */
    const timespanw time;

    /**
     * The writer flag that indicates if a writer has been attached.
     */
    bool writer;

public:
    typedef Event<T> event_t;

    /**
     * Instantiates a new RTML_buffer.
     */
    RTML_buffer();

    /**
     * Push a node
     */
    bool push(Event<T>&) ;

	/**
	 * Pull node in FIFO order
	 */
	bool pop(Event<T>&) ;

    /**
     * Get node at index
     */
	bool read(Event<T>&, size_t&) const;

    /**
     * Gets the static length of the buffer.
     *
     * @return the template parameter N.
     */
    size_t length() const;

    /**
     *
     */
    timespanw timespan() const { return time; };

    /**
     * Debugs the buffer into the stdout
     */
    void debug() const;
};

template<typename T, size_t N>
RTML_buffer<T, N>::RTML_buffer() :
    size(0),
    array(),
    writer(false),
	time(0)
{

}

template<typename T, size_t N>
bool RTML_buffer<T, N>::push(Event<T>& node) {
  array[size++] = node;
}

template<typename T, size_t N>
bool RTML_buffer<T, N>::pop(Event<T>& event) {
  if (size > 0)
    event = array[--size];

  return size > 0 ? true : false;
}

template<typename T, size_t N>
bool RTML_buffer<T, N>::read(Event<T>& event, size_t& index) const {
  event = array[index];

  return index < N ? true : false;
}

template<typename T, size_t N>
size_t RTML_buffer<T, N>::length() const {
    return size;
}

template<typename T, size_t N>
void RTML_buffer<T, N>::debug() const
{
    DEBUGV3(" ");
    for (unsigned int idx=0; idx < N; idx++)
        DEBUGV3_APPEND("%lu,%d; ", array[idx].ev.getTime(), array[idx].ev.getData());
    DEBUGV3_APPEND("\n");
}

#endif //_RTML_BUFFER_H_
