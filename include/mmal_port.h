#ifndef MMAL_PIC_PORT_H
#define MMAL_PIC_PORT_H

#include <interface/mmal/mmal_port.h>
#include <interface/mmal/mmal_pool.h>

#include "mmal_functions.h"
#include "mmal_types.h"
#include "support.h"

namespace mmal {

/// Base port
template <PORT_TYPE_ T_>
class Port_base_ {

protected:

    static const PORT_TYPE_ TYPE_ = T_;

public:

    /// ctor & dtor.
    Port_base_() = default;
    Port_base_(Port_base_&&) = default;
    Port_base_(const Port_base_&) = default;
    Port_base_& operator= (const Port_base_&) = default;
    Port_base_& operator= (Port_base_&&) = default;

    Port_base_(MMAL_PORT_T* port) :
        port_(port),
        pool_(nullptr)
    {}

    virtual ~Port_base_()
    {
        if (is_enabled())
            disable();
        if (pool_)
            destroy_pool();
    }

    /********************************* BASE FUNCTIONS *******************************/

    /**
     * @brief Enable the port by setting
     *        a callback function.
     */
    template <typename F_>
    void
    enable(F_&& cb_)
    { mmal_impl_::enable_port_(port_, cb_); }

    /**
     * @brief Disable the port.
     */
    void
    disable()
    { mmal_impl_::disable_port_(port_); }

    /**
     * @brief Check if the port is enabled or not.
     */
    bool
    is_enabled()
    { return port_->is_enabled; }

    /**
     * @brief Return the port's position within
     *        its owning type (INPUT, OUTPUT, ...).
     */
    uint16_t
    index()
    { return port_->index; }

    /**
     * @brief Set recommended buffers.
     *        If recommended is 0 set min values.
     */
    void
    set_default_buffer()
    {
        (port_->buffer_num_recommended == 0) ?
                    port_->buffer_num = port_->buffer_num_min
                : port_->buffer_num = port_->buffer_num_recommended;
        (port_->buffer_size_recommended == 0) ?
                    port_->buffer_size = port_->buffer_size_min
                : port_->buffer_size = port_->buffer_size_recommended;
    }

    /**
     * @brief Get buffer_size_min
     */
    uint32_t
    buffer_size_min() const
    { return port_->buffer_size_min; }

    /**
     * @brief Get buffer_num_min
     */
    uint32_t
    buffer_num_min() const
    { return port_->buffer_num_min; }

    /**
     * @brief Get buffer_size_recommended
     */
    uint32_t
    buffer_size_recommended() const
    { return port_->buffer_size_recommended; }

    /**
     * @brief Get buffer_num_recommended
     */
    uint32_t
    buffer_num_recommended() const
    { return port_->buffer_num_recommended; }

    /**
     * @brief Set buffer_size parameter
     */
    void
    set_buffer_size(uint32_t size)
    { port_->buffer_size = size; }

    /**
     * @brief Set buffer_num parameter
     */
    void
    set_buffer_num(uint32_t num)
    { port_->buffer_num = num; }

    /****************************** PARAMETERS FUNCTIONS ****************************/

    /**
     * @brief Set parameters to the port.
     */
    void
    set_parameters(MMAL_PARAMETER_HEADER_T* hdr_)
    { mmal_impl_::set_parameters_to_port_(port_, hdr_); }

    /**
     * @brief Set userdata pointer.
     */
    template<typename U_>
    void
    set_userdata(U_& userdata_)
    { this->port_->userdata = reinterpret_cast<
                MMAL_PORT_USERDATA_T*>(&userdata_); }

    /**
     * @brief Set boolean parameters.
     */
    void
    set_boolean_parameter(uint32_t id_, bool value_)
    { mmal_impl_::set_boolean_to_port_(port_, id_, (value_) ? 1 : 0); }

    /**
     * @brief Set int32 parameter.
     */
    void
    set_int32_parameter(uint32_t id_, int32_t value_)
    { mmal_impl_::set_int32_to_port_(port_, id_, value_); }

    /**
     * @brief Set uint32 parameter.
     */
    void
    set_uint32_parameter(uint32_t id_, uint32_t value_)
    { mmal_impl_::set_uint32_to_port_(port_, id_, value_); }

    /**
     * @brief Set int32 parameter.
     */
    void
    set_int64_parameter(uint32_t id_, int64_t value_)
    { mmal_impl_::set_int64_to_port_(port_, id_, value_); }

    /**
     * @brief Set uint64 parameter.
     */
    void
    set_int32_parameter(uint32_t id_, uint64_t value_)
    { mmal_impl_::set_int64_to_port_(port_, id_, value_); }

    /**
     * @brief Set rational parameter.
     */
    void
    set_rational_parameter(uint32_t id_, int32_t num_, int32_t den_)
    { mmal_impl_::set_rational_to_port_(port_, id_, num_, den_); }

    /**
     * @brief Set string parameter.
     */
    void
    set_string_parameter(uint32_t id_, const std::string& value_)
    { mmal_impl_::set_string_to_port_(port_, id_, value_); }

    /****************************** FORMAT FUNCTIONS ********************************/

    /**
     * @brief Commit format changes on this port.
     */
    void
    commit()
    { mmal_impl_::commit_format_(port_); }

    /**
     * @brief Copy format from another port.
     *        Note that the extradata buffer
     *        will not be copied.
     */
    template <PORT_TYPE_ TP_>
    void
    copy_from(Port_base_<TP_>& src_)
    { mmal_format_copy(port_->format, src_.format()); }

    /******************************** POOL FUNCTIONS ********************************/

    /**
     * @brief Create a pool using port's buffer_size
     *        and buffer_num.
     */
    void
    create_pool()
    { pool_ = mmal_pool_create(port_->buffer_num,
                               port_->buffer_size); }

    /**
     * @brief Send all buffer from the pool to the port.
     */
    void
    send_pool()
    { mmal_impl_::send_pool_to_port(port_, pool_); }

    /**
     * @brief Destroy a pool.
     */
    void
    destroy_pool()
    {
        mmal_port_pool_destroy(port_, pool_);
        pool_ = nullptr;
    }

    /******************************* MEMBER ACCESS **********************************/

    /**
     * @brief Get a pointer to the MMAL_PORT_T port.
     */
    MMAL_PORT_T*
    get()
    { return port_; }

    /**
     * @brief Get MMAL_ES_FORMAT_T pointer.
     */
    MMAL_ES_FORMAT_T*
    format()
    { return port_->format; }

    /**
     * @brief Get a pointer to the pool.
     */
    MMAL_POOL_T*
    pool()
    { return pool_; }

    /********************************************************************************/

protected:
    MMAL_PORT_T* port_;
    MMAL_POOL_T* pool_;

};

/// Generic port.
template<PORT_TYPE_ T_>
class Port : public Port_base_<T_> {

public:

    /// ctor & dtor.
    Port(MMAL_PORT_T* port) : Port_base_<T_>(port) {}
    Port() : Port_base_<T_>() {}
    Port(Port&&) = default;
    Port(const Port&) = default;
    Port& operator= (const Port&) = default;
    Port& operator= (Port&&) = default;
    ~Port() = default;

};


/// Specialization for output port.
template<>
class Port<PORT_TYPE_::INPUT>
        : public Port_base_<PORT_TYPE_::INPUT> {

public:

    /// ctor & dtor.
    Port(MMAL_PORT_T* port)
        : Port_base_<PORT_TYPE_::INPUT>(port),
          connection_(nullptr)
    {}

    Port()
        : Port_base_<PORT_TYPE_::INPUT>(),
          connection_(nullptr)
    {}

    Port(Port&&) = default;
    Port(const Port&) = default;
    Port& operator= (const Port&) = default;
    Port& operator= (Port&&) = default;

    ~Port(); /* virtual: implemented outside the class to avoid out-of-line warning. */

    /****************************** CONNECTION METHOD *******************************/

    /**
     * @brief Set a reference to the connection.
     */
    void
    set_connection(MMAL_CONNECTION_T* connection)
    { connection_ = connection; }

    /**
     * @brief Enable the connection.
     */
    void
    enable_connection()
    { mmal_impl_::enable_connection_(connection_); }

    /**
     * @brief Disable connection between the two ports.
     */
    void
    disable_connection()
    { mmal_impl_::disable_connection_(connection_); }

    /**
     * @brief Destroy the connection between the two ports.
     */
    void
    disconnect()
    {
        mmal_impl_::release_connection_(connection_);
        connection_ = nullptr;
    }

    /********************************************************************************/

private:
    MMAL_CONNECTION_T* connection_;

};

// dtor.
Port<PORT_TYPE_::INPUT>::~Port()
{
    if (connection_) {
        if (connection_->is_enabled)
            disable_connection();
        disconnect();
    }
}

/// Specialization for output port.
template<>
class Port<PORT_TYPE_::OUTPUT>
        : public Port_base_<PORT_TYPE_::OUTPUT> {

public:

    /// ctor & dtor.
    Port(MMAL_PORT_T* port)
        : Port_base_<PORT_TYPE_::OUTPUT>(port),
          connection_(nullptr)
    {}

    Port()
        : Port_base_<PORT_TYPE_::OUTPUT>(),
          connection_(nullptr)
    {}

    Port(Port&&) = default;
    Port(const Port&) = default;
    Port& operator= (const Port&) = default;
    Port& operator= (Port&&) = default;

    ~Port(); /* virtual: implemented outside the class to avoid out-of-line warning. */

    /****************************** CONNECTION METHOD *******************************/
    /**
     * @brief Create a connection between two ports.
     */
    void
    connect_to(Port<PORT_TYPE_::INPUT>& port,
               uint32_t flags)
    {
        connection_ = mmal_impl_::create_connection_(port_,
                                                   port.get(),
                                                   flags);
        port.set_connection(connection_);
    }

    /**
     * @brief Enable the connection.
     */
    void
    enable_connection()
    { mmal_impl_::enable_connection_(connection_); }

    /**
     * @brief Disable connection between the two ports.
     */
    void
    disable_connection()
    { mmal_impl_::disable_connection_(connection_); }

    /**
     * @brief Destroy the connection between the two ports.
     */
    void
    disconnect()
    {
        mmal_impl_::release_connection_(connection_);
        connection_ = nullptr;
    }

    /********************************************************************************/


private:
    MMAL_CONNECTION_T* connection_;

};

// dtor.
Port<PORT_TYPE_::OUTPUT>::~Port()
{
    if (connection_) {
        if (connection_->is_enabled)
            disable_connection();
        disconnect();
    }
}

};

#endif // MMAL_PIC_PORT_H
