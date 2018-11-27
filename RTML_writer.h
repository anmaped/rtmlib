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

#ifndef _RTEML_WRITER_H_
#define _RTEML_WRITER_H_

#ifdef USE_UNSAFE_METHODS
#include <list>
#endif

#include "Event.h"
#include "Ring_buffer.h"

/**
 * Writes events to a Ring_buffer.
 *
 * @see Ring_buffer
 *
 * @author André Pedro
 * @date
 */
template<typename T>
class RTML_writer {
private:
    /**
     * Points to a ring buffer this RTML_writer writes to.
     *
     * @see Ring_buffer
     */
    Ring_buffer<T> *const buffer;

    /*
     * Reference to the local page frame. Each writer will provide a proper
     * frame to be swaped with the global frame.
     */
    typedef Ring_buffer<T> cb;
    typename cb::tm_page page;
    typename cb::tm_page &page_ref = page;

public:

    /**
     * Instantiates a new RTML_writer.
     *
     * @param buffer the Buffer to write to.
     */
    RTML_writer(Ring_buffer<T> *const buffer);

    /**
    * enqueues an event to the Buffer.
    *
    * @param data a constant reference to the data to be pushed.
    */
    void enqueue(const T &data);

#ifdef USE_UNSAFE_METHODS
    void unsafe_enqueue(const size_t &index, const T &data, const timespan &t);
    void unsafe_enqueue_n(const std::list<std::pair<T,timespan>> &lst);
#endif

    /**
    * Sets this RTML_writer Buffer.
    *
    * Called during RTML_writer configuration by the RTML_buffer.
    *
    * @param buffer the buffer to configure this EventReader to.
    */
    void setBuffer(Ring_buffer<T> *buffer);
};

template<typename T>
RTML_writer<T>::RTML_writer(Ring_buffer<T> *const bbuffer) : buffer(bbuffer), page(0)
{

}

template<typename T>
void RTML_writer<T>::enqueue(const T &data) {
    buffer->enqueue(data, page_ref);
}

#ifdef USE_UNSAFE_METHODS
template<typename T>
void RTML_writer<T>::unsafe_enqueue(const size_t &index, const T &data, const timespan &t) {
    // copy the element (data,t) to the buffer position index
    buffer->writeEvent(data, t, index);
}

template<typename T>
void RTML_writer<T>::unsafe_enqueue_n(const std::list<std::pair<T,timespan>> &lst) {
    buffer->resetFrameCounter();
    buffer->resetFrameTimestamp();
    // check if there is space left
    if ( ! ( buffer->getLength() >= lst.size() ) )
        DEBUGV3("unsafe_enqueue_n is out of range\n");
    // copy all elements from the list into the buffer
    for (auto it = lst.begin(); it != lst.end(); it++)
        buffer->writeEvent((*it).first, (*it).second, buffer->counterToIndex(buffer->getCounterId()));
}
#endif

template<typename T>
void RTML_writer<T>::setBuffer(Ring_buffer<T> * const _buffer) {
    buffer = _buffer;
}

#endif //_RTEML_WRITER_H_
