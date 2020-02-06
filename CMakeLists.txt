cmake_minimum_required(VERSION 3.12)

project("MMALPP" VERSION 0.1 DESCRIPTION "A simple header only library that provide an interface with the broadcom MMAL library on the Raspberry PI to manage Components, Ports, Buffers, Queues, Pools and Connections."
                 HOMEPAGE_URL "https://github.com/s4mu313/MMALPP")

add_library(MMALPP INTERFACE)

target_include_directories(MMALPP INTERFACE mmalpp/)

install(DIRECTORY mmalpp DESTINATION test)
