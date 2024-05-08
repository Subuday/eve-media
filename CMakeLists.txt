cmake_minimum_required(VERSION 3.10)
project(eve-media)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DNO_TLS)

include_directories(lib/websocketpp)

add_executable(
    eve_media
    src/main.cpp
    src/App.cpp
    src/NetworkClient.cpp
    src/MediaClient.cpp
    src/utils/Semaphore.cpp
    src/utils/CountDownLatch.cpp
)

target_include_directories(eve_media PUBLIC include)

target_link_libraries(
    eve_media
    PRIVATE ssl
    PRIVATE crypto
    PRIVATE pulse
    PRIVATE pthread
)
