# MMALPP
----
MMAL (Multi-Media Abstraction Layer) is a C library that provides a relatively low-level interface to multimedia components running on VideoCore.
MMALPP a simple header-only library for Raspberry PI built over MMAL, whose purpose is to simplify the management of Components, Ports, Buffers, Pools, Queues and Connections.
This library is not perfect since is not fully tested, so expect unexpected behaviors or bugs.

# Prerequisites
----
  - MMAL installed on Raspberry PI
  - CMake >= 3.12

# Installation
----
1. Clone the repository.
   ```sh
   git clone https://github.com/s4mu313/MMALPP.git
   ```
2. Configure the project.
   ```sh
   cd MMALPP
   mkdir build && cd build
   cmake ..
   ```
3. Install the library.
   ```sh
   make
   make install
   ```

# Documentation
----

This library consists of six classes:

* <a href=#component>Component</a>
* <a href=#port>Port </a>
* <a href=#pool>Pool </a>
* <a href=#queue>Queue </a>
* <a href=#buffer>Buffer </a>
* <a href=#connection>Connection </a>

<h2 id="component">Component</h2>
This class represents a *MMAL_COMPONENT*. The constructor accepts a string that contains the name of the component to be initialized ("vc.ril.encoder", "vc.ril.camera", ...).

#### Methods

* **Component(const std::string& name)**: *create a component by setting its name.*
* **is_null() const**: *return true if the component pointer is null, false otherwise.*

* **is_enable() const**: *return true if the component is enabled, false otherwise.*
* **enable()**: *enable the component.*
* **disable()**: *disable the component.*
* **disconnect()**: *disconnect all connections from all the output ports of the component. This must be called always before the close method if you have connected any port.*
* **inputs() const**: *get the input port's number.*
* **outputs() const**: *get the output port's number.*
* **output(uint16_t num)**: *get a Port< OUTPUT > object representing the output port specified by num.*
* **input(uint16_t num)**: *get a Port< INPUT > object representing the input port specified by num.*
* **control()**: *get a Port< CONTROL > object representing the control port.*


<h2 id="port">Port</h2>

This class represents a *MMAL_PORT*. It is a templated class, you don't need to instantiate it directly, because it is initialized by a Component at the moment of creation with its correct type (output, input, control or clock).

#### Methods

* **is_null() const**: *return true if the port pointer is null, false otherwise.*

* **is_enable() const**: *return true if the port is enabled, false otherwise.*

* **enable(callback)**: *enable the port by setting a callback. The callback must be a void function that accepts two parameters, a Generic_port& reference and a Buffer object. They are explained below. The callback must not capture anything.*

* **commit()**: *Commit changes to the port's format.*
* **copy_from(const Generic_port& port)**: *Check if this Port is enabled.*
* **type()**: *Get Port's type.*
* **index()**: *Get index of this port in its type list.*
* **capabilities()**: *Get capabilities of the port.*
* **flush()**: *Ask a port to release all the buffer headers it currently has. Flushing a port will ask the port to send all the buffer headers it currently has to the client. Flushing is an asynchronous request and the flush call will return before all the buffer headers are returned to the client. It is up to the client to keep a count on the buffer headers to know when the flush operation has completed. It is also important to note that flushing will also reset the state of the port and any processing which was buffered by the port will be lost.*
* **buffer_num()**: *Get buffers number of the port.*
* **buffer_size()**: *Get buffer size of the port.*
* **buffer_num_min()**: *Get minimum buffers number of the port.*
* **buffer_size_min()**: *Get minimum buffers size of the port.*
* **buffer_num_recommended()**: *Get the recommended buffers number of the port.*
* **buffer_size_recommended()**: *Get recommended buffer size of the port.*
* **set_default_buffer()**: *Set buffer_num and buffer_size to recommended value. If recommended values are 0, they will be set to minimum values.*
* **send_buffer(const Buffer& buffer)**: *Send a Buffer to this port.*
* **parameter()**: *Get a Parameter instance to set port's parameter. Parameter is a class that allow to set parameters to the port.*
* **get()**: *Get a MMAL_PORT_T* pointer.*
* **format()**: *Get port's format.*
* **set_userdata(U& u)**: *Set userdata to the port.*
* **get_userdata_as\<U>()**: *Get userdata from the port already casted to U type.*
* **create_pool()**: *Create a Pool and associate it with this port.*
* **pool()**: *Get the Pool associated with this port.*
* **send_all_buffers()**: *Send all Buffer on the associated Pool to this port.*
* **release_pool()**: *Destroy the Pool associated with this Port.*
* **connection()**: *Get a reference to the Connection object.*
* **connect_to(Port\<INPUT>& target, uint32_t flags = 0)**: *Only in Port\<OUTPUT> port. This method connects an output port to an input port by creating a MMAL_CONNECTION between them.*

<h2 id="pool">Pool</h2>

This class represents a *MMAL_POOL*. It can be initialized either with a pointer to a MMAL_POOL or by specifying headers' number and size.

#### Methods
* **Pool(std::size_t headers, uint32_t size)**: *Construct the object.*
* <b>Pool(MMAL_POOL_T* pool)</b> : *Construct the object.*
* **release()**: *Release all Buffers in the Pool.*
* **is_null()**: *return true if the pool pointer is null, false otherwise.*
* **is_enable()**: *return true if the pool is enabled, false otherwise.*
* **get_buffer(int timeout_ms = 0)**: *Get a Buffer from a queue. If a timeout is greater than 0 it will wait up to timeout, then will abort if nothing is returned. If timeout is 0 it will get a Buffer without waiting. If timeout is less than 0 the function will block until a Buffer will be available.*

* **queue()**: *Get the Queue associated with the Pool.*
* **get()**: *Get the MMAL_POOL_T pointer.*
* **resize(std::size_t headers, uint32_t size)**: *Resize the Pool by specifying Buffers number and size.*
* **operator[](uint32_t n)**: *Access to the Pool and get the n-th Buffer from it.*
* **size()**: *Get the number of Buffers in the Pool.*


<h2 id="queue">Queue</h2>

This class represents a *MMAL_QUEUE*. This class can be created without arguments, in that case it will create a new queue, otherwise you can pass a pointer to a queue, and in that case it will be a wrapper of an existing MMAL_QUEUE.

#### Methods

* **is_null()**: *return true if the queue pointer is null, false otherwise*

* **release()**: *Release the Queue.*
* **size()**: *Get the number of Buffers in the Queue.*
* **put(const Buffer& buffer)**: *Put a Buffer into a queue.*
* **put_back(const Buffer& buffer)**: *Put back a Buffer into a queue.*
* **get_buffer(int timeout_ms = 0)**: *Get a Buffer from the queue.*


<h2 id="buffer">Buffer</h2>

This class represents a *MMAL_BUFFER_HEADER*. It is an iterable object that provides access to buffer data as a vector of uint8.

#### Methods

* **acquire()**: *Acquire a buffer header. Acquiring a buffer header increases a reference counter on it and makes sure that the buffer header won't be recycled until all the references to it are gone. If you call acquire you should call release before destroy the object, otherwise it will be memory leak.*
* **release()**: *Release a buffer header. Use this if you have previously acquired one. Once all references have been released, the buffer will be recycled.*
* **copy_meta(const Buffer& buffer)**: *Copy meta-data of Buffer. It copies presentation timestamp, decoding timestamp, command, flags, type.*
* **copy_from(const Buffer& buffer)**: *Copy all fields from another Buffer. It copies presentation timestamp, decoding timestamp, command, flags, type and data too. This buffer must have sufficient size to store length bytes from the source buffer. This method implicitly sets offset to zero, and length to the number of bytes copied.*
* **replicate(const Buffer& src)**: *Replicates the source Buffer. This copies all fields from the source buffer, including the internal data pointer. In other words, after replication this buffer and the source buffer will share the same block of memory for data. The source buffer will also be referenced internally by this buffer and will only be recycled once this buffer is released.*
* **reset()**: *Resets all buffer header fields to default values.*
* **type()**: *Get type of Buffer.*
* **is_null()**: *return true if the buffer pointer is null, false otherwise.*
* **size()**: *Get the length of the payload in the Buffer.*
* **flags()**: *Get flags if this Buffer.*
* **offset()**: *Get offset position of this Buffer.*

* **command()**: *Get command of this Buffer. This is usually 0 for buffers returned by an encoder; typically this is only used by buffers sent to the callback of a control port.*
* **decoding_timestamp()**: *Get decoding timestamp.*
* **presentation_timestamp()**: *Get presentation timestamp.*
* **allocated_size()**: *Get allocated size for a single buffer.*
* **data()**: *Get a pointer to Buffer's data.*
* **operator[](uint32_t n)**: *Buffer hold data as a byte array. You can access to data by [] operator. This operator doesn't do bound check.*

* **begin()**: *return an iterator object pointing at the first element.*
* **end()**: *return an iterator object pointing at the last element.*
* **get()**: *Get the MMAL_BUFFER_HEADER_T pointer.*




<h2 id="connection">Connection</h2>

This class represents a *MMAL_CONNECTION*. You can create it by passing a pointer to an output port as source, and a pointer to an input port as target, or you can simply use the method connect_to in the output port object.

#### Methods

* <b>Connection(Port\<OUTPUT>* source, Port\<INPUT>* target, uint32_t flags = 0)</b>: *constructor*
* **is_null()**: *return true if the connection pointer is null, false otherwise.*
* **is_enabled()**: *Check if the connection is enabled.*

* **source()**: *Get a reference to the source port. (Read-only)*
* **target()**: *Get a reference to the target port. (Read-only)*
* **enable()**: *Enable the connection.*
* **disable()**: *Disable the connection.*
* **release()**: *Destroy the connection.*
* **get()**: *Get the MMAL_CONNECTION_T pointer.*








# License

MIT

# Example program
```
#include <iostream>
#include <fstream>

#include <mmalpp.h>

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

```
