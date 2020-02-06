#include <iostream>
#include <fstream>

#include "mmalpp.h"

int main()
{
    /// Define components
    mmalpp::Component camera("vc.ril.camera");
    mmalpp::Component encoder("vc.ril.image_encode");
    mmalpp::Component null_sink("vc.null_sink");

    MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T change_event_request =
    {
        {MMAL_PARAMETER_CHANGE_EVENT_REQUEST, sizeof(MMAL_PARAMETER_CHANGE_EVENT_REQUEST_T)},
        MMAL_PARAMETER_CAMERA_SETTINGS, 1
    };

    /// Set parameters
    camera.control().parameter().set_header(&change_event_request.hdr);

    /// Passing callback with lambda expression
    camera.control().enable([](mmalpp::Generic_port& port, mmalpp::Buffer buffer){
        if (port.is_enabled())
            buffer.release();
    });

    MMAL_PARAMETER_CAMERA_CONFIG_T camConfig = {
        {MMAL_PARAMETER_CAMERA_CONFIG, sizeof ( camConfig ) },
        3280, // max_stills_w
        2464, // max_stills_h
        1, // stills_yuv422
        1, // one_shot_stills
        3280, // max_preview_video_w
        2464, // max_preview_video_h
        3, // num_preview_video_frames
        0, // stills_capture_circular_buffer_height
        0, // fast_preview_resume
        MMAL_PARAM_TIMESTAMP_MODE_ZERO // use_stc_timestamp
    };

    /// Set parameter to the control port
    camera.control().parameter().set_header(&camConfig.hdr);

    /// Set buffer_size and buffer_num to dfu value in the 2nd output port (still port)
    camera.output(2).set_default_buffer();

    /// Get still port's format
    MMAL_ES_FORMAT_T * format = camera.output(2).format();
    format->encoding = MMAL_ENCODING_OPAQUE;
    format->encoding = MMAL_ENCODING_I420;
    format->es->video.width = 640;
    format->es->video.height = 480;
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = 640;
    format->es->video.crop.height = 480;
    format->es->video.frame_rate.num = 1;
    format->es->video.frame_rate.den = 1;

    /// Commit changes
    camera.output(2).commit();

    /// Enable component
    camera.enable();

    /// Connect OUTPUT port 0 (preview port) to a null_sink component.
    camera.output(0).connect_to(null_sink.input(0), MMAL_CONNECTION_FLAG_TUNNELLING
                                    | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable the connection created above.
    camera.output(0).connection().enable();

    /// Enable null_sink component.
    null_sink.enable();

    /// Connect 2nd OUTPUT port (still port) to the encoder component.
    camera.output(2).connect_to(encoder.input(0), MMAL_CONNECTION_FLAG_TUNNELLING
                                    | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable connection
    camera.output(2).connection().enable();

    /// Copy format from another port
    encoder.output(0).copy_from(encoder.input(0));

    /// Set the encoder format to something (JPEG for example)
    encoder.output(0).format()->encoding = MMAL_ENCODING_JPEG;
    encoder.output(0).format()->encoding_variant = MMAL_ENCODING_BMP;

    /// Commit encoder's format changes
    encoder.output(0).commit();

    /// Define the callback that the encoder will call for each buffer (as a lambda)
    encoder.output(0).enable([](mmalpp::Generic_port& port, mmalpp::Buffer buffer){

        std::vector<u_char>& v = port.get_userdata_as<std::vector<u_char>>();

        /// Insert data into a vector.
        v.insert(v.end(), buffer.begin(), buffer.end());

        /// Notify when finished, You have to implement your notify_end_frame logic
        /// (e.g.: condition variable, mutex ,...).
        if (buffer.flags() & (MMAL_BUFFER_HEADER_FLAG_EOS |
                              MMAL_BUFFER_HEADER_FLAG_FRAME_END))
            std::cout << "Finished. Press ENTER to continue." << std::endl;

        buffer.release();

        if (port.is_enabled())
            port.send_buffer(port.pool().queue().get_buffer());

    });

    /// Create a pool based on buffer_size and buffer_num value of the
    /// port which is associated to.
    encoder.output(0).create_pool(encoder.output(0).buffer_num_recommended(),
                                  encoder.output(0).buffer_size_recommended());

    /// Enable the encoder.
    encoder.enable();

    /// Set userdata to the port so that you can get it from the callback.
    /// Of course you can pass a struct or a class or any kind of data you want.
    std::vector<u_char> v;
    encoder.output(0).set_userdata(v);

    MMAL_PARAMETER_EXPOSUREMODE_T exp_mode =
    {
        {
            MMAL_PARAMETER_EXPOSURE_MODE,
            sizeof ( exp_mode )
        },
        MMAL_PARAM_EXPOSUREMODE_AUTO
    };

    /// Set some exposure mode parameter.
    camera.control().parameter().set_header(&exp_mode.hdr);

    /// Send the pool created above to the port.
    encoder.output(0).send_all_buffers();

    /// Start capture just one frame
    camera.output(2).parameter().set_boolean(MMAL_PARAMETER_CAPTURE, true);

    std::cout << "Started" << std::endl;
    std::cin.get(); /// Implement your waiting_for_frame-logic.

    /// When frame is captured it continues and write on file.
    std::cout << "Writing on file" << std::endl;

    std ::ofstream f("test.jpg", std::ios::binary);
    f.write(reinterpret_cast<char*>(v.data()), static_cast<std::streamsize>(v.size()));
    f.close();

    /// Before calling close you must disconnect all connections previously created.
    null_sink.disconnect();
    encoder.disconnect();
    camera.disconnect();

    /// By calling close you will disable all ports and pools of each component.
    null_sink.close();
    encoder.close();
    camera.close();

    return 0;
}
