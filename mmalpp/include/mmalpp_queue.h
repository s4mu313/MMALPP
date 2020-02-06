#ifndef MMALPP_QUEUE_H
#define MMALPP_QUEUE_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_queue.h>

#include "mmalpp_buffer.h"
#include "utils/mmalpp_queue_utils.h"
#include "../macros.h"

MMALPP_BEGIN

class Queue {
public:

    /// ctors.
    Queue()
        : queue_(mmalpp_impl_::create_queue_())
    {}

    Queue(MMAL_QUEUE_T* queue)
        : queue_(queue)
    {}

    /**
     * Check if Queue exists.
     */
    bool
    is_null() const
    { return queue_ == nullptr; }

    /**
     * Release the Queue.
     */
    void
    release()
    { mmalpp_impl_::release_queue_(queue_); }

    /**
     * Get the number of Buffers in the Queue.
     */
    std::size_t
    size() const
    { return mmalpp_impl_::get_queue_lenght_(queue_); }

    /**
     * Put a Buffer into a queue.
     */
    void
    put(const Buffer& buffer)
    { mmalpp_impl_::put_in_queue_(queue_, buffer.get()); }

    /**
     * Put back a Buffer into a queue.
     */
    void
    put_back(const Buffer& buffer)
    { mmalpp_impl_::put_back_in_queue_(queue_, buffer.get()); }

    /**
     * Get a Buffer from the queue.
     */
    Buffer
    get_buffer(int timeout_ms = 0)
    { return mmalpp_impl_::get_buffer_from_queue_(queue_, timeout_ms); }

private:
    MMAL_QUEUE_T* queue_;

};

MMALPP_END

#endif // MMALPP_QUEUE_H
