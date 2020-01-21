/// NOTE: THIS LIB IS IN-DEV SO IT'S NOT COMPLETE.
/// IF YOU WANT TO TRY TO COMPILE THIS EXAMPLE YOU HAVE TO ADD SUPPORT TO:
/// c++1z, std::thread and this libs: -lmmal_core -lmmal -lmmal_util
/// -lmmal_components -lmmal_vc_client -lbcm_host -lvcos.
///
/// This is just one example of how you can use this OPENMAX / MMAL C++ WRAPPER.
/// It's only the first commit, so probably there are (even if I hope not)
/// a tons of bugs.


#include <fstream>
#include <iostream>
#include <condition_variable>

#include "include/mmal_component.h"

/// Simple Userdata struct to pass objects to the port in the callback
struct Userdata {
    std::condition_variable& cv;
    MMAL_POOL_T* p;
    std::vector<u_char>& v;
    Userdata(std::condition_variable& cv_,
             MMAL_POOL_T* p_,
             std::vector<u_char>& v_)
        : cv(cv_), p(p_), v(v_)
    {}
};

int main()
{
    /// Define components
    mmal::Component camera("vc.ril.camera");
    mmal::Component encoder("vc.ril.image_encode");
    mmal::Component null_sink("vc.null_sink");

    MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request =
    {
        {MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
        MMAL_PARAMETER_CAMERA_SETTINGS, 1
    };

    /// Set parameters
    camera.get_control().set_parameters(&change_event_request.hdr);

    /// Passing callback with lambda expression
    camera.get_control().enable([](MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer){
        mmal_buffer_header_release(buffer);
    });

    MMAL_PARAMETER_CAMERA_CONFIG_T camConfig = {
        {MMAL_PARAMETER_CAMERA_CONFIG, sizeof ( camConfig ) },
        1920, // max_stills_w
        1080, // max_stills_h
        1, // stills_yuv422
        1, // one_shot_stills
        1920, // max_preview_video_w
        1080, // max_preview_video_h
        3, // num_preview_video_frames
        0, // stills_capture_circular_buffer_height
        0, // fast_preview_resume
        MMAL_PARAM_TIMESTAMP_MODE_ZERO // use_stc_timestamp
    };

    /// Set parameter to the control port
    camera.get_control().set_parameters(&camConfig.hdr);

    /// Set buffer_size and buffer_num to dfu value in the 2nd output port (still port)
    camera.get_output(2).set_default_buffer();

    /// Get still port's format
    MMAL_ES_FORMAT_T * format = camera.get_output(2).format();
    format->encoding = MMAL_ENCODING_OPAQUE;
    format->encoding = MMAL_ENCODING_I420;
    format->es->video.width = 1920;
    format->es->video.height = 1080;
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = 1920;
    format->es->video.crop.height = 1080;
    format->es->video.frame_rate.num = 1;
    format->es->video.frame_rate.den = 1;

    /// Commit changes
    camera.get_output(2).commit();

    /// Enable component
    camera.enable();

    /// Connect OUTPUT port of a component to INPUT port of another one.
    /// Connect OUTPUT port 0 (preview port) to a null_sink component.
    camera.get_output(0).connect_to(null_sink.get_input(0), MMAL_CONNECTION_FLAG_TUNNELLING
                                    | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable the connection created above.
    camera.get_output(0).enable_connection();

    /// Enable null_sink component.
    null_sink.enable();

    /// Connect 2nd OUTPUT port (still port) to the encoder component.
    camera.get_output(2).connect_to(encoder.get_input(0), MMAL_CONNECTION_FLAG_TUNNELLING
                                    | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable connection
    camera.get_output(2).enable_connection();

    /// Copy format from another port
    encoder.get_output(0).copy_from(encoder.get_input(0));

    /// Set the encoder format to something (BMP for example)
    encoder.get_output(0).format()->encoding = MMAL_ENCODING_BMP;
    encoder.get_output(0).format()->encoding_variant = MMAL_ENCODING_I420;

    /// Commit encoder's format changes
    encoder.get_output(0).commit();

    /// Define the callback that the encoder will call for every buffer (as a lambda)
    encoder.get_output(0).enable([](MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer){
        Userdata* u = reinterpret_cast<Userdata*>(port->userdata);
        std::vector<u_char>& v = u->v;
        MMAL_POOL_T* p = u->p;
        std::condition_variable& cv = u->cv;

        /// Insert data into a vector.
        v.insert(v.end(), buffer->data, buffer->data + buffer->length);

        /// Notify when finished.
        if (buffer->flags & (MMAL_BUFFER_HEADER_FLAG_EOS))
            cv.notify_one();

        mmal_buffer_header_release(buffer);

        if (port->is_enabled)
        {
            MMAL_BUFFER_HEADER_T* newbuffer = mmal_queue_get(p->queue);
            mmal_port_send_buffer(port, newbuffer);
        }
    });

    /// Create a pool based on buffer_size and buffer_num value of the
    /// port which is associated to.
    encoder.get_output(0).create_pool();

    /// Enable the encoder.
    encoder.enable();

    std::mutex m;
    std::condition_variable cv;
    std::vector<u_char> v;
    MMAL_POOL_T* p = encoder.get_output(0).pool();

    Userdata u(cv, p, v);

    /// Set userdata struct to the port so that you can get it from the callback
    encoder.get_output(0).set_userdata(u);

    MMAL_PARAMETER_EXPOSUREMODE_T exp_mode =
    {
        {
            MMAL_PARAMETER_EXPOSURE_MODE,
            sizeof ( exp_mode )
        },
        MMAL_PARAM_EXPOSUREMODE_AUTO
    };

    /// Set some exposure mode parameter.
    camera.get_control().set_parameters(&exp_mode.hdr);

    /// Send the pool created above to the port.
    encoder.get_output(0).send_pool();

    std::unique_lock<std::mutex>lck(m);

    /// Start capture just one frame
    camera.get_output(2).set_boolean_parameter(MMAL_PARAMETER_CAPTURE, true);

    std::cout << "Started" << std::endl;
    cv.wait(lck);

    /// When frame is captured it continues and write on file.
    std::cout << "Writing on file" << std::endl;

    std ::ofstream f("obj.bmp", std::ios::binary);
    f.write(reinterpret_cast<char*>(v.data()), static_cast<std::streamsize>(v.size()));
    f.close();

    /// Components, connections, pools, ports and all the other resources
    /// will be automatically released by dtors.

    return 0;
}
