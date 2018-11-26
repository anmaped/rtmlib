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

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdio.h>
#include <time.h>

#include "Event.h"
#include "atomic_compat.h"
#include "debug_compat.h"

/**
 * Ring buffer implementation to be used in RTML_reader and RTML_writer.
 *
 * This buffer receives a pre-allocated memory region with a fixed size and
 * does atomic operations over the data elements.
 *
 * @see Event.h
 *
 * @author André Pedro
 * @date
 */
template<typename T>
class CircularBuffer {
private:

    /*
     * This enumeration type identifies when buffer elements are in updating
     * or ready state.
     */
    enum st {UPDATING=0, READY};

    /*
     * Node's type for the elements inside the ring buffer. 
     */
    struct _node {
        std::atomic<st> state;
        Event<T> ev;

        _node() : state(READY), ev(0,0) {}
    };

    /* 
     * This union type is used to split a 64 wide integer into two equaly
     * divided variables; idx means the current index/counting and pg is the
     * pointer to the page that could be local or external. pg should be 64bit
     * wide when using 64bit architectures. This union type is not used in arm
     * architecture neither in x86_64.
     */
#ifdef USE_DOUBLE_CAS
    union cnt {
        NATIVE_ATOMIC_POINTER ct64;
        struct {
            NATIVE_POINTER_TYPE pg;
            NATIVE_POINTER_TYPE idx;
        };
    };
#endif

    /*
     * The enqueue operation requires a frame page for doing Compare-And-Swap
     * (CAS). The frame contains the absolute current time from the initial
     * registered clock. It can be days, years, months..., or relative e.g. the
     * processor clock. We assume time to be of type uint64_t.
     */
    struct _page {

        uint64_t time;
        size_t counter;

        _page(uint64_t t) :  time(t) {}
    };

    /*
     * Constant that saves the absolute time when buffer is initialized by
     * the aplication.
     */
    const timespanw initial_clock;

    /*
     * Stores the local page frame relative to this buffer instantiation.
     */
    struct _page local_page;
    
    /**
     * Stores a constant pointer to the begin of the buffer (it is normally
     * an array containing a continuous chunck of memory).
     */
    struct _node *const ca_accesspointer;

    /**
     * Stores the size of the available memory starting at ca_accesspointer. It
     * is constant along the usage of this CircularBuffer.
     */
    const size_t ca_length;

    /*
    * We implement two types of CAS. The double CAS is used for x86 and the
    * simple CAS to arm and x86_64 platforms. Note that simple CAS suffers from ABA
    * problem unlike double CAS that solves it. However, as we have implemented
    * it, our setting is not afected by such phonema since we swap distinct
    * addresses that were previously allocated, i.e., they are unique.
    */
    FRAME_ADDRESS_type FRAME_ADDRESS;
    FRAME_ADDRESS_type & frame = FRAME_ADDRESS;

    /*
     * Some helper functions for managing the frames.
     */
    size_t   getCounterValue(FRAME_ADDRESS_subtype counter) const;
    void     setCounterValue(FRAME_ADDRESS_subtype &counter, size_t idx) const;

    NATIVE_POINTER_TYPE getCounterCurrentPage(FRAME_ADDRESS_subtype counter) const;
    void     setCounterCurrentPage(FRAME_ADDRESS_subtype &counter, NATIVE_POINTER_TYPE t) const;

    timeabs getCounterCurrentTimestamp(FRAME_ADDRESS_subtype counter) const;
    void    setCounterCurrentTimestamp(FRAME_ADDRESS_subtype &lcounter, timeabs t) const;

public:

    typedef struct _node node;
    typedef struct _page tm_page;

    /**
     * Instantiates a new buffer using external memory allocation.
     *
     * @param array a reference to a constant pointer that points to the array.
     * @param length the length of the array.
     */
    CircularBuffer(node * const &array, const size_t length);

    /**
     * Atomically enqueues data into the circular buffer.
     *
     * Index is atomically updated after writing an element, and state
     * variables are also atomically swapped. A timestamp is attached when the
     * element is enqueued.
     *
     * @param data the data to be pushed.
     * @param page the page frame to be used during the swap.
     */
    void enqueue(const T &data, tm_page & page);

    /**
     * Reads an index from the buffer.
     *
     * @param event a reference to an event object where the data will be stored.
     * @param index the index to read from.
     */
    void readEvent(Event<T> &event, const size_t index) const;

    /**
     * Gets the state of the buffer.
     *
     * @param time the current time.
     * @param idx  the current index.
     */
    void getState(timeabs &time, size_t &idx) const;

    /**
    * Maps the counter to the circular array index.
    *
    * @param lcounter the counter to be mapped.
    */
    size_t counterToIndex(uint32_t lcounter) const;

    /**
     * Gets the number of enqueued events.
     *
     * @return the number of elements since buffer was initialized.
     */
    size_t getCounterId() const;

    /**
     * Gets the current index, i.e., the index where the writer last wrote.
     *
     * @return the index.
     */
    size_t getHead() const;

    /**
     * Checks the availability of the node to be read.
     *
     * @param idx the index of the node
     *
     * @return true if the node is available, and false otherwise
     */
    bool nodeIsReady(const size_t idx) const;

    /**
     * Gets the difference of the current time with the initial absolute time
     * when buffer is created.
     *
     * @param unaligned_time the time to be aligned with the initial clock
     * @return the time input time aligned
     */
    timespanw getTimeAlignment(timespanw unaligned_time) const;

    /**
     * Gets the array length.
     *
     * @return the array length.
     */
    size_t getLength() const { return ca_length; };

    /**
     * Gets the frame by reference.
     *
     * @return the frame reference.
     */
    FRAME_ADDRESS_type & getFrameReference() const { return frame; };

    #ifdef USE_UNSAFE_METHODS
        void writeEvent(T data, timespan t, const size_t index);
        void resetFrameCounter();
        void resetFrameTimestamp();
    #endif
};

template<typename T>
CircularBuffer<T>::CircularBuffer(node* const &array, const size_t length) :
    initial_clock(clockgettime()),
    local_page(initial_clock),
    ca_accesspointer(array),
    ca_length(length)
{
    FRAME_ADDRESS_subtype x;

    setCounterCurrentPage(x, (NATIVE_POINTER_TYPE)&local_page);
    setCounterValue(x, 0);

    getFrameReference().store(x);
}

template<typename T>
size_t CircularBuffer<T>::counterToIndex(uint32_t lcounter) const {
    return lcounter % getLength();
}

template<typename T>
size_t CircularBuffer<T>::getCounterValue(FRAME_ADDRESS_subtype lcounter) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    return x.idx;
#else
    tm_page * previous_tm_page = (tm_page *) lcounter;

    return previous_tm_page->counter;
#endif
}

template<typename T>
void CircularBuffer<T>::setCounterValue(FRAME_ADDRESS_subtype &lcounter, size_t idx) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    x.idx = idx;
    lcounter = x.ct64;
#else
    tm_page * previous_tm_page = (tm_page *) lcounter;
    previous_tm_page->counter = idx;
#endif
}

template<typename T>
NATIVE_POINTER_TYPE CircularBuffer<T>::getCounterCurrentPage(FRAME_ADDRESS_subtype lcounter) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    return x.pg;
#else
    return lcounter;
#endif
}

template<typename T>
void CircularBuffer<T>::setCounterCurrentPage(FRAME_ADDRESS_subtype &lcounter, NATIVE_POINTER_TYPE pg) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    x.pg = pg;
    lcounter = x.ct64;
#else
     lcounter = pg;
#endif
}

template<typename T>
timeabs CircularBuffer<T>::getCounterCurrentTimestamp(uint64_t lcounter) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    tm_page * previous_tm_page = (tm_page *) x.pg;
#else
    tm_page * previous_tm_page = (tm_page *) lcounter;
#endif

    return previous_tm_page->time;
}

template<typename T>
void CircularBuffer<T>::setCounterCurrentTimestamp(FRAME_ADDRESS_subtype &lcounter, timeabs t) const
{
#ifdef USE_DOUBLE_CAS
    union cnt x;
    x.ct64 = lcounter;
    tm_page * previous_tm_page = (tm_page *) x.pg;
    previous_tm_page->time = t;
#else
    tm_page * previous_tm_page = (tm_page *) lcounter;
    previous_tm_page->time = t;
#endif
}


template<typename T>
void CircularBuffer<T>::enqueue(const T &data, tm_page & page) {

    uint32_t tempCounter;
    size_t tempIndex;
    timeabs tmptime; // verify type ... was uint64_t --> change to timeabs
    timespan nextTime;
    uint32_t newtempCounter;
    Event<T> *tempEvent;
    tm_page * ntp;

    // this part need to be processed atomically (set timestamp and increment the counter)
    // since several less significant bytes are available in one 64 bit word, we will use
    // it to store the counter. the counter should be larger as much as the buffer size.
    FRAME_ADDRESS_subtype new_value = 0; // this value may be used with incorrect initialization [DO NOT MAKE CHANGES BELOW]

    // instructions for measure time and number of tries
    //START_MEASURE();

    ATOMIC_begin_VALUE64(getFrameReference());

        //COUNT_CYCLE();

        // lets increment and get the counter atomically
        tempCounter = getCounterValue(OLD_FRAME_ADDRESS);
        // map counter to buffer index
        tempIndex = counterToIndex(tempCounter);

        // get timestamp
        tmptime = clockgettime();

        DEBUGV3("Time:%lld - %lld = %lld : Pointer: %p _> %p\n",
            tmptime,
            getCounterCurrentTimestamp(OLD_FRAME_ADDRESS),
            tmptime - getCounterCurrentTimestamp(OLD_FRAME_ADDRESS),
            page,
            &local_page
        );

        nextTime = tmptime - getCounterCurrentTimestamp(OLD_FRAME_ADDRESS) ;

        // if page is currently in use then swap it with the local_page
        if ( getCounterCurrentPage(OLD_FRAME_ADDRESS) == &page )
        {
            DEBUGV3("using main page\n");
            ntp = &local_page;
        }
        else
        {
            ntp = &page;
        }

        ntp->time = tmptime;

        // lets create the new value using the old value with incremented counter and new page
        newtempCounter = tempCounter + 1;
        
        setCounterCurrentPage(new_value, ntp);
        setCounterValue(new_value, newtempCounter);

        // lets mark the event updating
        ca_accesspointer[tempIndex].state.store(UPDATING);

        DEBUGV3("Time:%lld -- %ld : Pointer: %p\n",
            ntp->time,
            nextTime,
            ntp
        );

    ATOMIC_end_VALUE64(new_value, getFrameReference());

    // now we know the new index and timestamp
    // lets modify the new event and set it to ready state

    tempEvent = &ca_accesspointer[tempIndex].ev;
    tempEvent->setTime( (timespan) nextTime );
    tempEvent->setData( data );

    ca_accesspointer[tempIndex].state.store(READY);

    //STOP_MEASURE();

    DEBUGV3("Counter is lock free:%d\n", atomic_is_lock_free(&getFrameReference()));
}

template<typename T>
void CircularBuffer<T>::readEvent(Event<T> &event, const size_t index) const
{
    event = ca_accesspointer[index].ev;
}

template<typename T>
void CircularBuffer<T>::getState(timeabs &time, size_t &idx) const
{
    /* Frame can be swapped when time and idx are retrieved; a guarantee for
     * frame swapping is ensured by comparing the frame address before and
     * after the assignments.
     */
    ATOMIC_begin_VALUE64_NOEXCHANGE(getFrameReference());
        time = getCounterCurrentTimestamp(OLD_FRAME_ADDRESS);
        idx = getCounterValue(OLD_FRAME_ADDRESS);
    ATOMIC_end_VALUE64_NOEXCHANGE(getFrameReference());
}

template<typename T>
size_t CircularBuffer<T>::getCounterId() const
{
    size_t idx;

    /* Frame can be swapped when frame and idx are retrieved; a guarantee for
     * frame swapping is ensured by comparing the frame address before and
     * after the assignment.
     */
    ATOMIC_begin_VALUE64_NOEXCHANGE(getFrameReference());
        idx = getCounterValue(OLD_FRAME_ADDRESS);
    ATOMIC_end_VALUE64_NOEXCHANGE(getFrameReference());

    return idx;
}

template<typename T>
size_t CircularBuffer<T>::getHead() const
{
    return counterToIndex(getCounterId());
}

template<typename T>
bool CircularBuffer<T>::nodeIsReady(const size_t idx) const
{
    return ca_accesspointer[idx].state.load() == READY;
}

template<typename T>
timespanw CircularBuffer<T>::getTimeAlignment(timespanw unaligned_time) const
{
    return (unaligned_time >= initial_clock)? unaligned_time - initial_clock : 0;
};


#ifdef USE_UNSAFE_METHODS
template<typename T>
void CircularBuffer<T>::writeEvent(T data, timespan t, const size_t index)
{
    ca_accesspointer[index].ev.setTime(t);
    ca_accesspointer[index].ev.setData(data);
    FRAME_ADDRESS_subtype f = frame.load();
    setCounterValue(f,getCounterValue(f) + 1);
    frame.store(f);
    local_page.time = local_page.time + t;
}

template<typename T>
void CircularBuffer<T>::resetFrameCounter()
{
    FRAME_ADDRESS_subtype f = frame.load();
    setCounterValue(f, 0);
    frame.store(f);
}

template<typename T>
void CircularBuffer<T>::resetFrameTimestamp()
{
    FRAME_ADDRESS_subtype f = frame.load();
    setCounterCurrentTimestamp(f, 0);
    frame.store(f);
}
#endif

#endif //_CIRCULAR_BUFFER_H_
