#ifndef MMALPP_POOL_H
#define MMALPP_POOL_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_pool.h>

#include "mmalpp_queue.h"
#include "utils/mmalpp_pool_utils.h"
#include "../macros.h"

MMALPP_BEGIN

class Pool {
public:

    /// ctors.
    Pool(MMAL_POOL_T* pool)
        : pool_(pool)
    {}

    Pool(std::size_t headers, uint32_t size)
        : pool_(mmalpp_impl_::create_pool_(headers, size))
    {}

    /**
     * Release all Buffers in the Pool.
     */
    void
    release()
    { mmalpp_impl_::pool_release_(pool_); pool_ = nullptr; }

    /**
     * Check if Pool exists.
     */
    bool
    is_null() const
    { return pool_ == nullptr; }

    /**
     * Get a Buffer from a queue. If a timeout is greater than 0 it will wait
     * up to timeout, then will abort if nothing is returned.
     * If timeout is 0 it will get a Buffer without waiting. If timeout is less than 0
     * the function will block until a Buffer will be available.
     */
    Buffer
    get_buffer(int timeout_ms = 0)
    { return mmalpp_impl_::get_buffer_from_queue_(pool_->queue, timeout_ms); }

    /**
     * Get the Queue associated with the Pool.
     */
    Queue
    queue() const
    { return {pool_->queue}; }

    /**
     * Get the number of Buffers in the Pool.
     */
    std::size_t
    size() const
    { return pool_->headers_num; }

    /**
     * Access to the Pool and get the n-th Buffer in it.
     * Attention: this function doesn't check bounds.
     */
    Buffer
    operator[](uint32_t n)
    { return pool_->header[n]; }

    /**
     * Access to the Pool and get the n-th Buffer in it.
     * Attention: this function doesn't check bounds.
     */
    const Buffer
    operator[](uint32_t n) const
    { return pool_->header[n]; }

    /**
     * Resize the Pool.
     */
    void
    resize(std::size_t headers, uint32_t size)
    { mmalpp_impl_::pool_resize_(pool_, headers, size); }

    /**
     * Get the MMAL_POOL_T pointer.
     */
    MMAL_POOL_T*
    get() const
    { return pool_; }

private:
    MMAL_POOL_T* pool_;

};

MMALPP_END

#endif // MMALPP_POOL_H
