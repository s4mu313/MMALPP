#ifndef SUPPORT_H
#define SUPPORT_H

#include <interface/mmal/mmal_port.h>
#include <interface/mmal/mmal_pool.h>

#include "mmal_functions.h"

namespace mmal_impl_ {

/**
 * @brief Send a pool to the specified port.
 */
void
send_pool_to_port(MMAL_PORT_T* port_,
                  MMAL_POOL_T* pool_)
{
    if (!pool_)
        throw std::runtime_error("error sending pool to port: " +
                                 std::string(port_->name) + " - pool_ is null");
    std::size_t length = mmal_queue_length(pool_->queue);
    for(std::size_t i = 0; i < length; ++i)
        send_buffer_(port_, mmal_queue_get(pool_->queue));
}

/**
 * @brief Set up all ports by filling the vector
 */
template <typename V_>
void
setup_ports_(V_& v_,
             MMAL_PORT_T** ports_,
             uint32_t n_)
{
    v_.reserve(n_);
    for(std::size_t i = 0; i < n_; ++i)
        v_.emplace_back(ports_[i]);
}

};

#endif // SUPPORT_H
