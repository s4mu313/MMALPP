#ifndef MMALPP_CONNECTION_H
#define MMALPP_CONNECTION_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/util/mmal_connection.h>

#include "mmalpp_port.h"
#include "mmalpp_fwd_decl.h"
#include "utils/mmalpp_connection_utils.h"
#include "../macros.h"

MMALPP_BEGIN

class Connection {

public:

    /// ctor.
    Connection(Port<OUTPUT>* source,
               Port<INPUT>* target,
               uint32_t flags = 0)
        : connection_(mmalpp_impl_::create_connection_(source->get(),
                                                       target->get(),
                                                       flags)),
          source_(source),
          target_(target)
    { target->connection_ = this; }

    /**
     * Check if it exists.
     */
    bool
    is_null()
    { return connection_ == nullptr; }

    /**
     * Check if it is enabled.
     */
    bool
    is_enabled()
    { return connection_->is_enabled; }

    /**
     * Get a reference to the source port. (Read-only)
     */
    const Port<OUTPUT>&
    source() const
    { return *source_; }

    /**
     * Get a reference to the target port. (Read-only)
     */
    const Port<INPUT>&
    target() const
    { return *target_; }

    /**
     * Enable the connection.
     */
    void
    enable()
    { mmalpp_impl_::enable_connection_(connection_); }

    /**
     * Disable the connection.
     */
    void
    disable()
    { mmalpp_impl_::disable_connection_(connection_); }

    /**
     * Destroy the connection.
     */
    void
    release()
    {
        mmalpp_impl_::destroy_connection_(connection_);
        target_->connection_ = nullptr;
    }

    /**
     * Get the MMAL_CONNECTION_T pointer.
     */
    MMAL_CONNECTION_T*
    get()
    { return connection_; }

private:
    MMAL_CONNECTION_T* connection_;
    Port<OUTPUT>* source_;
    Port<INPUT>* target_;

};

MMALPP_END

#endif // MMALPP_CONNECTION_H
