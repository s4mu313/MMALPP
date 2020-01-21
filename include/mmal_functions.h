#ifndef MMAL_FUNCTIONS_H
#define MMAL_FUNCTIONS_H

#include <interface/mmal/mmal_port.h>
#include <interface/mmal/mmal_pool.h>
#include <interface/mmal/util/mmal_util.h>
#include <interface/mmal/util/mmal_connection.h>
#include <interface/mmal/util/mmal_util_params.h>

#include "mmal_exceptions.h"

namespace mmal_impl_ {

/**
 * @brief Enable a MMAL_PORT_T port.
 *        Set a callback.
 */
void
enable_port_(MMAL_PORT_T* port_,
             MMAL_PORT_BH_CB_T cb_)
{ if (MMAL_STATUS_T status = mmal_port_enable(port_, cb_); status)
        e_check__(status, "cannot enable port"); }

/**
 * @brief Disable a MMAL_PORT_T port.
 */
void
disable_port_(MMAL_PORT_T* port_)
{ if (MMAL_STATUS_T status = mmal_port_disable(port_); status)
        e_check__(status, "cannot disable port"); }

/**
 * @brief Commit format changes to the port.
 */
void
commit_format_(MMAL_PORT_T* port_)
{ if (MMAL_STATUS_T status = mmal_port_format_commit(port_); status)
        e_check__(status, "cannot commit format on port " + std::string(port_->name)); }

/**
 * @brief Send a buffer to a port.
 */
void
send_buffer_(MMAL_PORT_T* port_,
            MMAL_BUFFER_HEADER_T* buffer_)
{  if (MMAL_STATUS_T status = mmal_port_send_buffer(port_, buffer_); status)
        e_check__(status, "cannot send buffer to the port " + std::string(port_->name)); }

/**
 * @brief Create a connection between two ports.
 */
MMAL_CONNECTION_T*
create_connection_(MMAL_PORT_T* src_,
                  MMAL_PORT_T* dst_,
                  uint32_t flags_ = 0)
{
    MMAL_CONNECTION_T* c_;
    if (MMAL_STATUS_T status = mmal_connection_create(&c_, src_, dst_, flags_); status) {
        mmal_connection_destroy(c_);
        e_check__(status, "cannot create a connection between " +
                  std::string(src_->name) + " and " + std::string(dst_->name));
    }
    return c_;
}

/**
 * @brief Enable a connection.
 */
void
enable_connection_(MMAL_CONNECTION_T* connection_)
{ if (MMAL_STATUS_T status = mmal_connection_enable(connection_); status)
        e_check__(status, "cannot enable connection between " +
                  std::string(connection_->out->name) + " and " +
                  std::string(connection_->in->name)); }

/**
 * @brief Disable a connection.
 */
void
disable_connection_(MMAL_CONNECTION_T* connection_)
{ if (MMAL_STATUS_T status = mmal_connection_disable(connection_); status)
        e_check__(status, "cannot disable connection between " +
                  std::string(connection_->out->name) + " and " +
                  std::string(connection_->in->name)); }

/**
 * @brief Release a connection.
 */
void
release_connection_(MMAL_CONNECTION_T* connection_)
{ if (MMAL_STATUS_T status = mmal_connection_destroy(connection_); status)
        e_check__(status, "cannot destroy connection between " +
                  std::string(connection_->out->name) + " and " +
                  std::string(connection_->in->name)); }

/**
 * @brief Create a new component.
 */
MMAL_COMPONENT_T*
create_component_(const char* name_)
{
    MMAL_COMPONENT_T* component_;
    if (MMAL_STATUS_T status = mmal_component_create(name_, &component_); status) {
        mmal_component_release(component_);
        e_check__(status, "cannot create component: " +
                  std::string(name_));
    }
    return component_;
}

/**
 * @brief Disable all ports and destroy a component.
 */
void
release_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_release(component_); status)
        e_check__(status, "cannot release component: " +
                  std::string(component_->name)); }

/**
 * @brief Enable a component.
 */
void
enable_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_enable(component_); status)
        e_check__(status, "cannot enable component: " +
                  std::string(component_->name)); }

/**
 * @brief Disable a component.
 */
void
disable_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_disable(component_); status)
        e_check__(status, "cannot disable component: " +
                  std::string(component_->name)); }

/**
 * @brief Set parameters to the port.
 */
void
set_parameters_to_port_(MMAL_PORT_T* port_,
                        MMAL_PARAMETER_HEADER_T* param_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set(port_, param_); status)
        e_check__(status, "cannot set parameter to the port"); }

/**
 * @brief Set boolean parameters to the port.
 */
void
set_boolean_to_port_(MMAL_PORT_T* port_,
                     uint32_t id_,
                     int32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_boolean(port_, id_, value_); status)
        e_check__(status, "cannot set boolean parameter to the port"); }

/**
 * @brief Set uint64 parameters to the port.
 */
void
set_uint64_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    uint64_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_uint64(port_, id_, value_); status)
        e_check__(status, "cannot set uint64_t parameter to the port"); }

/**
 * @brief Set int64 parameters to the port
 */
void
set_int64_to_port_(MMAL_PORT_T* port_,
                   uint32_t id_,
                   int64_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_int64(port_, id_, value_); status)
        e_check__(status, "cannot set int64_t parameter to the port"); }

/**
 * @brief Set uint32 parameters to the port
 */
void
set_uint32_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    uint32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_uint32(port_, id_, value_); status)
        e_check__(status, "cannot set uint32_t parameter to the port"); }

/**
 * @brief Set int32 parameters to the port
 */
void
set_int32_to_port_(MMAL_PORT_T* port_,
                   uint32_t id_,
                   int32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_int32(port_, id_, value_); status)
        e_check__(status, "cannot set int32_t parameter to the port"); }

/**
 * @brief Set rational parameters to the port
 */
void
set_rational_to_port_(MMAL_PORT_T* port_,
                      uint32_t id_,
                      int32_t num_,
                      int32_t den_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_rational(
                port_, id_, MMAL_RATIONAL_T{num_, den_}); status)
        e_check__(status, "cannot set MMAL_RATIONAL_T "
                          "({int32_t, int32_t}) parameter to the port"); }

/**
 * @brief Set string parameters to the port
 */
void
set_string_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    const std::string& value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_string(
                port_, id_, value_.c_str()); status)
        e_check__(status, "cannot set string parameter to the port"); }


};

#endif // MMAL_FUNCTIONS_H
