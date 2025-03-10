# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(HelloXComponent)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_definitions(-DOHOS_PLATFORM)
add_definitions(-DEGL_EGLEXT_PROTOTYPES=1)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(helloxcomponent SHARED
    common/log.h
    hello/BitmapRenderer.cpp
    hello/BitmapRenderer.h
    hello/DelegatedNodeContent.cpp
    hello/DelegatedNodeContent.h
    hello/GLCore.cpp
    hello/GLCore.h
    hello/NapiManager.cpp
    hello/NapiManager.h
    hello/NodeContent.cpp
    hello/NodeContent.h
    hello/NonDelegatedNodeContent.cpp
    hello/NonDelegatedNodeContent.h
    hello/TextureRenderer.cpp
    hello/TextureRenderer.h
    hello/Thread.cpp
    hello/Thread.h
    hello/XComponentNode.cpp
    hello/XComponentNode.h
    napi_init.cpp
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