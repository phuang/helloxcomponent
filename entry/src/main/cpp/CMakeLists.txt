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
    EGL-lib
    EGL
)

find_library(
    GLES-lib
    GLESv3
)

find_library(
    hilog-lib
    hilog_ndk.z
)

find_library(
    libace-lib
    ace_ndk.z
)

find_library(
    libnapi-lib
    ace_napi.z
)

find_library(
    libnative_buffer-lib
    native_buffer
)

find_library(
    libnative_window-lib
    native_window
)

find_library(
    libuv-lib
    uv
)

target_link_libraries(helloxcomponent PUBLIC
    ${EGL-lib}
    ${GLES-lib}
    ${hilog-lib}
    ${libace-lib}
    ${libnapi-lib}
    ${libnative_buffer-lib}
    ${libnative_window-lib}
    ${libuv-lib}
)