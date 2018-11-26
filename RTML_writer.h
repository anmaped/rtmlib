#ifndef _RTEML_WRITER_H_
#define _RTEML_WRITER_H_

#ifdef USE_UNSAFE_METHODS
#include <list>
#endif

#include "Event.h"
#include "CircularBuffer.h"

/**
 * Writes events to a RTML_buffer.
 *
 * @see RTML_buffer
 *
 * @author André Pedro (anmap@isep.ipp.pt)
 * @author Humberto Carvalho (1120409@isep.ipp.pt)
 * @date
 */
template<typename T>
class RTML_writer {
private:
    /**
     * Pointer to a circular buffer this RTML_writer writes to.
     * @see RTML_buffer
     */
    CircularBuffer<T> *const buffer;

    /*
     * Reference to the local page frame. Each writer will provide a proper
     * frame to be swaped with the global frame.
     */
    typedef CircularBuffer<T> cb;
    typename cb::tm_page new_tm_page;
    typename cb::tm_page & new_tm_page_ref = new_tm_page;

public:

    /**
     * Instantiates a new RTML_writer.
     *
     * @param buffer the Buffer to write to.
     */
    RTML_writer(CircularBuffer<T> *const buffer);

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
    void setBuffer(CircularBuffer<T> *buffer);
};

template<typename T>
RTML_writer<T>::RTML_writer(CircularBuffer<T> *const bbuffer) : buffer(bbuffer), new_tm_page(0)
{

}

template<typename T>
void RTML_writer<T>::enqueue(const T &data) {
    // lets use the available page
    buffer->enqueue(data, new_tm_page_ref);
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
void RTML_writer<T>::setBuffer(CircularBuffer<T> * const bbuffer) {
    //buffer = bbuffer;
}

#endif //_RTEML_WRITER_H_
