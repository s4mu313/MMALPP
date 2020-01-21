#ifndef MMAL_PIC_COMPONENT_H
#define MMAL_PIC_COMPONENT_H

#include "mmal_functions.h"
#include "mmal_types.h"
#include "mmal_port.h"

#include <vector>

namespace mmal {

class Component_base_ {

public:

    /// Deleted ctors.
    Component_base_(Component_base_&&) = delete;
    Component_base_(const Component_base_&) = delete;
    Component_base_& operator= (const Component_base_&) = delete;
    Component_base_& operator= (Component_base_&&) = delete;

    /* virtual dtor: implemented outside the class to avoid out-of-line warning. */
    virtual ~Component_base_();

    /// ctors.
    Component_base_() = default;

    Component_base_(const std::string& name)
        : component_(mmal_impl_::create_component_(name.c_str()))
    {}

    /******************************* COMMON METHODS *********************************/

    /**
     * @brief Enable the component.
     */
    void
    enable()
    { mmal_impl_::enable_component_(component_); }

    /**
     * @brief Disable the component.
     */
    void
    disable()
    { mmal_impl_::disable_component_(component_); }

    /**
     * @brief Enable the component.
     */
    bool
    is_enable() const
    { return component_->is_enabled; }

    /**
     * @brief Get input port's number.
     */
    uint32_t
    input_ports() const
    { return component_->input_num; }

    /**
     * @brief Get output port's number.
     */
    uint32_t
    output_ports() const
    { return component_->output_num; }


protected:
    MMAL_COMPONENT_T* component_;

};

/// virtual dtor.
Component_base_::~Component_base_()
{
    if (is_enable())
        disable();
    mmal_impl_::release_component_(component_);
}


class Component : public Component_base_ {

public:

    /// Deleted ctors.
    Component(Component&&) = delete;
    Component(const Component&) = delete;
    Component& operator= (const Component&) = delete;
    Component& operator= (Component&&) = delete;

    /* virtual dtor: implemented outside the class to avoid out-of-line warning. */
    ~Component();

    /// ctors.
    Component() = default;

    Component(const std::string& name)
        : Component_base_(name.c_str()),
          control_(component_->control)
    {
        mmal_impl_::setup_ports_(input_, component_->input, component_->input_num);
        mmal_impl_::setup_ports_(output_, component_->output, component_->output_num);
    }

    /********************************* ACCESS METHODS *******************************/

    /**
     * @brief Get output port reference.
     */
    Port<OUTPUT>&
    get_output(uint16_t num)
    { return output_[num]; }

    /**
     * @brief Get input port reference.
     */
    Port<INPUT>&
    get_input(uint16_t num)
    { return input_[num]; }

    /**
     * @brief Get control port reference.
     */
    Port<CONTROL>&
    get_control()
    { return control_; }

private:
    std::vector<Port<INPUT>> input_;
    std::vector<Port<OUTPUT>> output_;
    Port<CONTROL> control_;

};

/// virtual dtor.
Component::~Component()
{}



};

#endif // MMAL_PIC_COMPONENT_H
