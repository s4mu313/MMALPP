#ifndef MMALPP_COMPONENT_UTILS_H
#define MMALPP_COMPONENT_UTILS_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_component.h>

#include "exceptions/mmalpp_exceptions.h"
#include "../../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Create an instance of a component.
 * The newly created component will expose ports to the client. All the exposed ports are
 * disabled by default.
 * Note that components are reference counted and creating a component automatically
 * acquires a reference to it.
 */
inline MMAL_COMPONENT_T*
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
 * Release a reference on a component.
 * Release an acquired reference on a component. Triggers the destruction of the component when
 * the last reference is being released.
 */
inline void
release_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_release(component_); status)
        e_check__(status, "cannot release component: " +
                  std::string(component_->name)); }

/**
 * Enable processing on a component.
 */
inline void
enable_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_enable(component_); status)
        e_check__(status, "cannot enable component: " +
                  std::string(component_->name)); }

/**
 * Disable processing on a component.
 */
inline void
disable_component_(MMAL_COMPONENT_T* component_)
{ if (MMAL_STATUS_T status = mmal_component_disable(component_); status)
        e_check__(status, "cannot disable component: " +
                  std::string(component_->name)); }

};

MMALPP_END

#endif // MMALPP_COMPONENT_UTILS_H
