cmake_minimum_required(VERSION 3.10)
project(eve-media)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    main
    src/main.cpp
    src/MediaManager.cpp
)

target_include_directories(main PUBLIC include)

target_link_libraries(
    main
    pulse
    pthread
)