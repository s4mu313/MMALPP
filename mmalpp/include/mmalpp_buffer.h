#ifndef MMALPP_BUFFER_H
#define MMALPP_BUFFER_H

#include <iterator>

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_buffer.h>

#include "utils/mmalpp_buffer_utils.h"
#include "../macros.h"

MMALPP_BEGIN

/// Iterator
class Buffer_iterator {

public:

    using value_type = uint8_t;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = uint8_t*;
    using reference = uint8_t&;
    using const_reference = const uint8_t&;

    /// ctor.
    Buffer_iterator(uint8_t* data,
                    uint32_t length,
                    bool end = false)
        : data_(data),
          length_(length),
          pos_((end) ? length - 1 : 0)
    {}

    /// Pre-increment.
    Buffer_iterator&
    operator++()
    {
        if (pos_ + 1 < length_)
            ++pos_;
        return *this;
    }

    /// Post-increment.
    Buffer_iterator
    operator++(int)
    {
        Buffer_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    /// Access non-const.
    reference
    operator*()
    { return data_[pos_]; }

    /// Access const.
    const_reference
    operator*() const
    { return data_[pos_]; }

private:
    const pointer data_; /// data ptr
    const std::size_t length_; /// length
    std::size_t pos_; /// actual position

};

/******************************************************************/

/// operator== overload.
inline bool
operator==(const Buffer_iterator& a,
           const Buffer_iterator& b)
{ return &*a == &*b; }

/// operator!= overload.
inline bool
operator!=(const Buffer_iterator& a,
           const Buffer_iterator& b)
{ return !(a == b); }

/******************************************************************/

/// Buffer class.
class Buffer {

public:

    using value_type = uint8_t;
    using reference = uint8_t&;
    using const_reference = const uint8_t&;
    using iterator = Buffer_iterator;
    using const_iterator = const Buffer_iterator;

    /// ctor.
    Buffer(MMAL_BUFFER_HEADER_T* buffer)
        : buffer_(buffer)
    {}

    Buffer() = default;
    Buffer(const Buffer&) = default;
    Buffer(Buffer&&) = default;
    Buffer& operator=(Buffer&& ) = default;
    Buffer& operator=(const Buffer& ) = default;

    /**
     * Acquire a buffer header. Acquiring a buffer header increases a reference counter
     * on it and makes sure that the buffer header won't be recycled until all the references
     * to it are gone. If you call acquire you should call release before destroy the object,
     * otherwise it will be memory leak.
     */
    void
    acquire()
    { mmalpp_impl_::acquire_buffer_header_(buffer_); }

    /**
     * Release a buffer header. Use this if you have previously acquired one.
     * Once all references have been released, the buffer will be recycled.
     */
    void
    release()
    { mmalpp_impl_::release_buffer_header_(buffer_); }

    /**
     * Copy meta-data of Buffer. It copies presentation timestamp, decoding timestamp,
     * command, flags, type.
     */
    void
    copy_meta(const Buffer& buffer)
    {
        buffer_->pts = buffer.presentation_timestamp();
        buffer_->dts = buffer.decoding_timestamp();
        buffer_->cmd = buffer.command();
        buffer_->flags = buffer.flags();
        *buffer_->type = buffer.type();
    }

    /**
     * Copy all fields from another Buffer. It copies presentation timestamp,
     * decoding timestamp, command, flags, type and data too.
     * This buffer must have sufficient size to store length bytes from the source buffer.
     * This method implicitly sets offset to zero, and length to the number of bytes copied.
     */
    void
    copy_from(const Buffer& buffer)
    {
        if (allocated_size() < buffer.allocated_size())
            throw std::length_error("Not enough allocated memory to store data. "
                                    "Actual: " + std::to_string(allocated_size()) +
                                    "Required: " + std::to_string(buffer.allocated_size()));
        std::copy(buffer.begin(), buffer.end(), begin());
        copy_meta(buffer);
        buffer_->offset = 0;
        buffer_->length = buffer.size();
    }
    /**
     * Replicates the source Buffer. This copies all fields from the source buffer,
     * including the internal data pointer. In other words, after replication this buffer
     * and the source buffer will share the same block of memory for data.
     * The source buffer will also be referenced internally by this buffer and will
     * only be recycled once this buffer is released.
     */
    void
    replicate(const Buffer& src)
    { mmalpp_impl_::replicate_buffer_header_(src.get(), get()); }

    /**
     * Resets all buffer header fields to default values.
     */
    void
    reset()
    { mmalpp_impl_::reset_buffer_header_(buffer_); }

    /**
     * Get type of Buffer.
     */
    MMAL_BUFFER_HEADER_TYPE_SPECIFIC_T
    type() const
    { return *buffer_->type; }

    /**
     * Check is this Buffer exists.
     */
    bool
    is_null() const
    { return buffer_ == nullptr; }

    /**
     * Get the length of the payload in the Buffer.
     */
    uint32_t
    size() const
    { return buffer_->length; }

    /**
     * Get flags if this Buffer.
     */
    uint32_t
    flags() const
    { return buffer_->flags; }

    /**
     * Get offset position of this Buffer.
     */
    uint32_t
    offset() const
    { return buffer_->offset; }

    /**
     * Get command of this Buffer. This is usually 0 for buffers returned
     * by an encoder; typically this is only used by buffers sent to the
     * callback of a control port.
     */
    uint32_t
    command() const
    { return buffer_->cmd; }

    /**
     * Get decoding timestamp.
     */
    int64_t
    decoding_timestamp() const
    { return buffer_->dts; }

    /**
     * Get presentation timestamp.
     */
    int64_t
    presentation_timestamp() const
    { return buffer_->pts; }

    /**
     * Get allocated size.
     */
    uint32_t
    allocated_size() const
    { return buffer_->alloc_size; }

    /**
     * Get a const pointer to Buffer's data.
     */
    const uint8_t*
    data() const
    { return buffer_->data; }

    /**
     * Get a pointer to Buffer's data.
     */
    uint8_t*
    data()
    { return buffer_->data; }

    /**
     * Buffer hold data as a byte array.
     * You can have access to data pointer by [] operator.
     * This operator doesn't do bound check.
     */
    uint8_t&
    operator[](uint32_t n)
    { return *(buffer_->data + n); }

    /**
     * Buffer hold data as a byte array.
     * You can have access to data pointer by [] operator.
     * This operator doesn't do bound check.
     */
    const uint8_t&
    operator[](uint32_t n) const
    { return *(buffer_->data + n); }

    /**
     * Begin iterator.
     */
    iterator
    begin()
    { return {buffer_->data, buffer_->alloc_size}; }

    /**
     * End iterator.
     */
    iterator
    end()
    { return {buffer_->data, buffer_->alloc_size, true}; }

    /**
     * Begin const-iterator.
     */
    const_iterator
    begin() const
    { return {buffer_->data, buffer_->alloc_size}; }

    /**
     * End const-iterator.
     */
    const_iterator
    end() const
    { return {buffer_->data, buffer_->alloc_size, true}; }

    /**
     * Get the MMAL_BUFFER_HEADER_T pointer.
     */
    MMAL_BUFFER_HEADER_T*
    get() const
    { return buffer_; }

private:
    MMAL_BUFFER_HEADER_T* buffer_;

};

/// USED FOR DEBUG
inline std::ostream& operator<<(std::ostream& os, const Buffer& b)
{
    for (auto i = b.begin(); i != b.end(); ++i)
        os << *i;
    return os;
}

MMALPP_END

#endif // MMALPP_BUFFER_H
