#ifndef MMALPP_COMPONENT_H
#define MMALPP_COMPONENT_H

#include <string>

#include "mmalpp_port.h"
#include "mmalpp_connection.h"
#include "utils/mmalpp_component_utils.h"
#include "mmalpp_support.h"
#include "../macros.h"

MMALPP_BEGIN

class Component {

public:

    /// ctor.
    Component(const std::string& name)
        : component_(mmalpp_impl_::create_component_(name.c_str())),
          control_(component_->control)
    {
        mmalpp_impl_::setup_ports_(input_, component_->input, component_->input_num);
        mmalpp_impl_::setup_ports_(output_, component_->output, component_->output_num);
    }

    /**
     * Close component. Disable all ports, release all pools associated
     * with the ports and destroy the component.
     */
    void
    close()
    {
        mmalpp_impl_::close_(output_);
        mmalpp_impl_::close_(input_);
        mmalpp_impl_::close_single_(control_);
        mmalpp_impl_::release_component_(component_);
    }

    /**
     * Disconnect this component from others. This method must be call
     * for each component before call the close method.
     */
    void
    disconnect()
    { mmalpp_impl_::disconnect_ports_(output_); }

    /**
     * Check if it exists.
     */
    bool
    is_null() const
    { return component_ == nullptr; }

    /**
     * Enable component
     */
    void
    enable()
    { mmalpp_impl_::enable_component_(component_); }

    /**
     * Disable component
     */
    void
    disable()
    { mmalpp_impl_::disable_component_(component_); }

    /**
     * Check if it is enabled.
     */
    bool
    is_enable() const
    { return component_->is_enabled; }

    /**
     * Get input port number.
     */
    uint32_t
    inputs() const
    { return component_->input_num; }

    /**
     * Get output port number.
     */
    uint32_t
    outputs() const
    { return component_->output_num; }

    /**
     * Get n-th output port.
     */
    Port<OUTPUT>&
    output(uint16_t num)
    { return output_[num]; }

    /**
     * Get n-th input port.
     */
    Port<INPUT>&
    input(uint16_t num)
    { return input_[num]; }

    /**
     * Get control port.
     */
    Port<CONTROL>&
    control()
    { return control_; }

private:

    MMAL_COMPONENT_T* component_;

    std::vector<Port<INPUT>> input_;
    std::vector<Port<OUTPUT>> output_;
    Port<CONTROL> control_;

};

MMALPP_END

#endif // MMALPP_COMPONENT_H
