cmake_minimum_required(VERSION 3.10)
project(eve-media)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_PYTHON_BINDINGS "Build Python Bindings" OFF)

# cadd_definitions(-DNO_TLS)

include_directories(lib/websocketpp)

add_subdirectory(lib/pybind11)

if(BUILD_PYTHON_BINDINGS)
    pybind11_add_module(
        eve_media
        src/main.cpp
        src/App.cpp
        src/NetworkClient.cpp
        src/MediaClient.cpp
        src/python_bindings.cpp
        src/utils/Semaphore.cpp
    )
else()
    add_executable(
        eve_media
        src/main.cpp
        src/App.cpp
        src/NetworkClient.cpp
        src/MediaClient.cpp
        src/utils/Semaphore.cpp
    )
endif()

target_include_directories(eve_media PUBLIC include)

target_link_libraries(
    eve_media
    PRIVATE ssl
    PRIVATE crypto
    PRIVATE pulse
    PRIVATE pthread
)
