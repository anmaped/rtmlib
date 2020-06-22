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

#ifndef _RTEML_READER_H_
#define _RTEML_READER_H_

#include <utility>
#include <array>
#include <tuple>
#include <time.h>

#include "Event.h"
#include "RTML_buffer.h"


enum state_rd_t {AVAILABLE, AVAILABLE_PARTIALLY, UNAVAILABLE, OVERWRITTEN};


/**
 * Reads events from a RTML_buffer.
 *
 * @author André Pedro
 * @date
 */
template<typename B>
class RTML_reader {
private:
    /**  Constant pointer to a ring Buffer this RTML_reader reads from.
     * @see RTML_buffer
     */
    const B& buffer;

    /** Number of readed elements */
    size_t size;

    /** The timestamp of the last event this buffer read.
     * @see time_compat.h
     */
    timeabs timestamp;

public:

    typedef B buffer_t;

    /**
     * Instantiates a new RTML_reader.
     *
     * Instantiates a new event reader that reads from buffer.
     *
     * @param buffer a constant pointer that points to a constant buffer.
     */
    RTML_reader(const B&);

    /**
     * Dequeues the next event from the buffer.
     *
     * @param idx the optional index to dequeue
     *
     * @return a pair
     *
     */
    std::pair<state_rd_t,typename B::event_t > dequeue(int idx = -1);

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
    size_t getHigherIdx() const { return buffer.length(); }

    /**
     * Gets the buffer state.
     *
     * @return a pair consisting by one absolute timestamp and one index
     */
    std::pair<timeabs, size_t> getCurrentBufferState() const {
    	timeabs time;
        size_t idx;
        // [TODO]
        // buffer->getState(time, idx);
        return std::make_pair (time,idx);
    };

    timespanw getTimeAlignment(timespanw time) const { return (time >= buffer.timespan())? time - buffer.timespan() : 0; }
};

template<typename B>
RTML_reader<B>::RTML_reader(const B& _buffer) :
    buffer(_buffer),
    size(0),
    timestamp(0)
{

}

template<typename B>
std::pair<state_rd_t, typename B::event_t > RTML_reader<B>::dequeue(int idx) {

    typename B::event_t tempEvent;
    size_t n_elems_writer;

    //size_t index_for_event = (idx == -1) ? buffer->counterToIndex(size) : idx;
    size_t index_for_event = idx;

    // atomic operation block       >####
    
    //ATOMIC_begin_VALUE64_NOEXCHANGE(buffer->getFrameReference());

        buffer.read(tempEvent, index_for_event);  // unsafe in terms of empty buffer
        //n_elems_writer = buffer->getCounterId();
        n_elems_writer = 10; // [TODO]

        /*if (!buffer->nodeIsReady(index_for_event))
        {
            continue;
        }*/

    //ATOMIC_end_VALUE64_NOEXCHANGE(buffer->getFrameReference());

    // end of atomic operation block >####
    
    // continue processing ...

    if (size < n_elems_writer)
    {
        // measure the distance between the indexes of the reader and writer
        // is greater than buffer length (this indicate an event overwrite)
        if(n_elems_writer-size > buffer.length())
        {
            // when there is an overwrite of the events
            return std::make_pair (OVERWRITTEN, tempEvent);
        }

        if(idx == -1)
        {
            size ++; // locally increment the number of read elements

            // update absolute time state of the monitor
            timestamp += tempEvent.getTime();
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

template<typename B>
bool RTML_reader<B>::synchronize()
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

    //DEBUGV("time:%llu,%llu\n", g_ts, lastread_ts);
    //DEBUGV("idx:%lu,%lu\n", g_index, n_elems_reader);

    if (g_ts > timestamp)
    {
        //DEBUGV("lost elements:%d\n", g_index - n_elems_reader);

        timestamp = g_ts;
        size = g_index; // this is not the best time stamp (it skips the old events of the buffer)

        return true;
    }
    else
        return false; // more time is required; synchronization is for now impossible.
}

template<typename B>
bool RTML_reader<B>::isConsistent() const
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
    if ( g_index - size > buffer->getLength() )
    {
        // there is a blind segment
        timeabs dif = g_ts - timestamp; // elapsed time

        return false;
    }

    return true;
}

#endif //_RTEML_READER_H_
