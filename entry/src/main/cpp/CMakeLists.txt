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
    hello/BitmapRenderer.cpp
    hello/BitmapRenderer.h
    hello/XComponentNode.cpp
    hello/XComponentNode.h
    napi_init.cpp
    napi_manager.cpp
    napi_manager.h
)

target_link_libraries(helloxcomponent PUBLIC
    EGL
    GLESv3
    ace_ndk.z
    ace_napi.z
    hilog_ndk.z
    image_source
    native_buffer
    native_window
    pixelmap
    uv
)