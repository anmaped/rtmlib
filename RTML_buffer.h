/*
 *  rtmlib is a Real-Time Monitoring Library.
 *  This library was initially developed in CISTER Research Centre as a proof
 *  of concept by the current developer and, since then it has been freely
 *  maintained and improved by the original developer.
 *
 *    Copyright (C) 2018 André Pedro
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
#include <time.h>

#include "Ring_buffer.h"
#include "RTML_writer.h"
#include "RTML_reader.h"

/**
 * RTML_buffer is able to support applications and monitors that requires to
 * share the same buffer by splinting operations to read and write, e.g.
 * the monitor uses the RTML_reader, and the RTML_writer can be used for
 * instrumentation.
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
     * The Event array where events are kept. Size is defined via template
     * parameter N, avoiding dynamic memory usage.
     *
     * @see Event
     */
    typedef Ring_buffer<T> cb;
    typename cb::node array[N];

    /**
     * The infinite buffer that is used for readers and writers of the RTEML.
     *
     * @see Ring_buffer
     */
    Ring_buffer<T> buffer;

    /**
     * The writer flag that indicates if a writer has been attached.
     */
    bool writer;

public:
    /**
     * Instantiates a new RTML_buffer.
     */
    RTML_buffer();

    /**
     * Gets the static length of the buffer.
     *
     * @return the template parameter N.
     */
    size_t getLength() const;

    Ring_buffer<T> * getBuffer() const;

    /**
     * Debugs the buffer into the stdout
     */
    void debug() const;
};

template<typename T, size_t N>
RTML_buffer<T, N>::RTML_buffer() :
    array(),
    buffer(array, N),
    writer(false)
{

}

template<typename T, size_t N>
size_t RTML_buffer<T, N>::getLength() const {
    return N;
}

template<typename T, size_t N>
Ring_buffer<T> * RTML_buffer<T, N>::getBuffer() const {
    return (Ring_buffer<T> *)&buffer;
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
