#ifndef _RTML_BUFFER_H_
#define _RTML_BUFFER_H_

#include <stdio.h>
#include <time.h>

#include "CircularBuffer.h"
#include "RTML_writer.h"
#include "RTML_reader.h"

/**
 * RTML_buffer allows instrumented applications and monitors to share the
 * the same buffer by splinting it into read and write operations.
 *
 * Monitor uses RTML_reader, and RTML_writer is reserved for software modules
 * under observation.
 *
 * @see Event
 * @see RTML_reader
 * @see RTML_monitor
 *
 * @author André Pedro (anmap@isep.ipp.pt)
 * @author Humberto Carvalho (1129498@isep.ipp.pt)
 * @date
 */
template<typename T, size_t N>
class RTML_buffer {
private:
    /**
     * The Event array where events are kept. Size is defined via template
     * parameter N, avoiding dynamic memory usage.
     * @see Event
     */
    typedef CircularBuffer<T> cb;
    typename cb::node array[N];

    /**
     * The infinite buffer that is used for readers and writers of the RTEML.
     * @see CircularBuffer
     */
    CircularBuffer<T> buffer;

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

    CircularBuffer<T> * getBuffer() const;

    /**
     * Debugs the infinite buffer into the stdout
     */
    void debug() const;
};

template<typename T, size_t N>
RTML_buffer<T, N>::RTML_buffer() :
    buffer(array, N),
    writer(false)
{

}

template<typename T, size_t N>
size_t RTML_buffer<T, N>::getLength() const {
    return N;
}

template<typename T, size_t N>
CircularBuffer<T> * RTML_buffer<T, N>::getBuffer() const {
    return (CircularBuffer<T> *)&buffer;
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
