# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(HelloXComponent)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DOHOS_PLATFORM)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(helloxcomponent SHARED
    common/log.h
    hello/XComponentNode.cpp
    hello/XComponentNode.h
    napi_init.cpp
    napi_manager.cpp
    napi_manager.h
    )

find_library(
    libEGL
    EGL
)

find_library(
    libGLES
    GLESv3
)

find_library(
    libhilog
    hilog_ndk.z
)

find_library(
    libace_ndk
    ace_ndk.z
)

find_library(
    libace_napi
    ace_napi.z
)

find_library(
    libnative_buffer
    native_buffer
)

find_library(
    libnative_window
    native_window
)

find_library(
    libuv
    uv
)

target_link_libraries(helloxcomponent PUBLIC
    ${libEGL}
    ${libGLES}
    ${libhilog}
    ${libace_ndk}
    ${libace_napi}
    ${libnative_buffer}
    ${libnative_window}
    ${libuv}
)