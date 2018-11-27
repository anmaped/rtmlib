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

#ifndef _RTEML_READER_H_
#define _RTEML_READER_H_

#include <utility>
#include <array>
#include <tuple>

#include <time.h>
#include "Event.h"
#include "Ring_buffer.h"

#define LOCAL_BUFFER_SIZE 10
#define DEQUEUE_SIZE(size) dequeue_## size
#define DEQUEUE_N DEQUEUE_SIZE(LOCAL_BUFFER_SIZE)

#define dequeue_n_declaration(size) \
    std::tuple<state_rd_t, size_t, std::array<Event<T>, size> > \
        dequeue_##size (int idx = -1)

#define dequeue_n(size) \
    std::tuple<state_rd_t, size_t, std::array<Event<T>, size> > RTML_reader<T>::dequeue_##size (int idx) \
    { \
        std::array<Event<T>, size> local_buffer; 


enum state_rd_t {AVAILABLE, AVAILABLE_PARTIALLY, UNAVAILABLE, OVERWRITTEN};


/**
 * Reads events from a Ring_buffer.
 *
 * @author André Pedro
 * @date
 */
template<typename T>
class RTML_reader {
private:
    /**  Constant pointer to a ring Buffer this RTML_reader reads from.
     * @see Ring_buffer
     */
    const Ring_buffer<T> *buffer;

    /** Number of readed elements */
    size_t n_elems_reader;

    /** The timestamp of the last event this buffer read.
     * @see time_compat.h
     */
    timeabs lastread_ts;

public:

    /**
     * Instantiates a new RTML_reader.
     *
     * Instantiates a new event reader that reads from buffer.
     *
     * @param buffer a constant pointer that points to a constant buffer.
     */
    RTML_reader(const Ring_buffer<T> * buffer);

    /**
     * Dequeues the next event from the buffer.
     *
     * @param idx the optional index to dequeue
     *
     * @return a pair
     *
     */
    std::pair<state_rd_t,Event<T> > dequeue(int idx = -1);

    /**
     * Dequeues n events
     *
     * @param idx the optional index to dequeue
     *
     * @return a tuple consisting of the code, the number of dequeue events, and the array with those events
     */
    dequeue_n_declaration(20);

    /**
     * Synchronizes the RTML_reader index according to a timestamp
     *
     * @param time defines the timestamp to syncronize.
     *
     * @return true if the RTML_reader was synchronized, false otherwise.
     */
    bool synchronize();

    /**
     * Compares the current RTML_reader absolute timestamp with the
     * current absolute timestamp of the buffer.
     *
     * @return true if matched, false otherwise.
     */
    bool isConsistent() const;


    /**
     * Gets the lower bound.
     *
     * @return 0
     */
    size_t getLowerIdx() const { return 0; }

    /**
     * Gets the upper bound.
     *
     * @return an unsigned integer
     */
    size_t getHigherIdx() const { return buffer->getLength(); }

    /**
     * Gets the buffer state.
     *
     * @return a pair consisting by one absolute timestamp and one index
     */
    std::pair<timeabs, size_t> getCurrentBufferState() const { timeabs time;
        size_t idx; buffer->getState(time, idx);
        return std::make_pair (time,idx);
    };

    timespanw getTimeAlignment(timespanw time) const { return buffer->getTimeAlignment(time); }
};

template<typename T>
RTML_reader<T>::RTML_reader(const Ring_buffer<T> * bbuffer) :
    buffer(bbuffer),
    n_elems_reader(0),
    lastread_ts(0)
{

}

template<typename T>
std::pair<state_rd_t,Event<T> > RTML_reader<T>::dequeue(int idx) {

    Event<T> tempEvent;
    size_t n_elems_writer;

    size_t index_for_event = (idx == -1) ? buffer->counterToIndex(n_elems_reader) : idx;

    // atomic operation block       >####
    
    ATOMIC_begin_VALUE64_NOEXCHANGE(buffer->getFrameReference());

        buffer->readEvent(tempEvent, index_for_event);  // unsafe in terms of empty buffer
        n_elems_writer = buffer->getCounterId();

        if (!buffer->nodeIsReady(index_for_event))
        {
            continue;
        }

    ATOMIC_end_VALUE64_NOEXCHANGE(buffer->getFrameReference());

    // end of atomic operation block >####
    
    // continue processing ...

    if (n_elems_reader < n_elems_writer)
    {
        // measure the distance between the indexes of the reader and writer
        // is greater than buffer length (this indicate an event overwrite)
        if(n_elems_writer-n_elems_reader > buffer->getLength())
        {
            // when there is an overwrite of the events
            return std::make_pair (OVERWRITTEN, tempEvent);
        }

        if(idx == -1)
        {
            n_elems_reader ++; // locally increment the number of read elements

            // update absolute time state of the monitor
            lastread_ts += tempEvent.getTime();
        }

        // when successful
        return std::make_pair (AVAILABLE, tempEvent);
    }
    else
    {
        // when there is no event available
        return std::make_pair (UNAVAILABLE, tempEvent);
    }
    
}

template<typename T>
dequeue_n(20)
    
    Event<T> tmpEvent;
    state_rd_t code;
    std::pair<state_rd_t,Event<int> > tuple;

    // lets dequeue a set of AVAILABLE events at the same time using dequeue operation
    for(uint32_t i=0; i<local_buffer.size(); i++)
    {
        // dequeue event as a tuple consisting by one event and one code.
        if(idx == -1)
            tuple = dequeue();
        else
            tuple = dequeue(idx + i);

        tmpEvent = tuple.second;
        code = tuple.first;

        if (code != AVAILABLE)
        {
            if(i == 0)
            {
                if(code == OVERWRITTEN)
                    return std::make_tuple (OVERWRITTEN, 0, local_buffer);
                else
                    return std::make_tuple (UNAVAILABLE, 0, local_buffer);
            }
            else
                return std::make_tuple (AVAILABLE_PARTIALLY, i, local_buffer);
        }
        
        // add event time to the current reader absolute time
        local_buffer[i].setTime(tmpEvent.getTime());

        // store event data in the buffer
        local_buffer[i].setData(tmpEvent.getData());
        
    }

    return std::make_tuple (AVAILABLE, local_buffer.size(), local_buffer);
}

template<typename T>
bool RTML_reader<T>::synchronize()
{
    timeabs g_ts;
    size_t g_index;

    // One synchronization can be consistent or inconsistent. It depends on the buffer length.
    // any event that is overwritten without being read turns the buffer inconsistent.

    // get the state of the buffer atomically
    auto pair = getCurrentBufferState();

    // evaluates the state
    g_ts = pair.first; // global timestamp
    g_index = pair.second; // global index --- note that this index can overload ... How to treat that circumstances?

    DEBUGV("time:%llu,%llu\n", g_ts, lastread_ts);
    DEBUGV("idx:%lu,%lu\n", g_index, n_elems_reader);

    if (g_ts > lastread_ts)
    {
        DEBUGV("lost elements:%d\n", g_index - n_elems_reader);

        lastread_ts = g_ts;
        n_elems_reader = g_index; // this is not the best time stamp (it skips the old events of the buffer)

        return true;
    }
    else
        return false; // more time is required; synchronization is for now impossible.
}

template<typename T>
bool RTML_reader<T>::isConsistent() const
{
    timeabs g_ts;
    size_t g_index;

    // are timestamps close enough ?
    // buffer is enough to avoid overwrite of events?
    // consistency check consists on analyzing the current indexes and timestamps of the buffer and reader

    // get the state of the buffer atomically
    auto pair = getCurrentBufferState();

    // evaluates the state
    g_ts = pair.first; // global timestamp
    g_index = pair.second; // global index --- note that this index can overload ... How to treat that circumstances?

    // test for trace blinding
    if ( g_index - n_elems_reader > buffer->getLength() )
    {
        // there is a blind segment
        timeabs dif = g_ts - lastread_ts; // elapsed time

        return false;
    }

    return true;
}

#endif //_RTEML_READER_H_
