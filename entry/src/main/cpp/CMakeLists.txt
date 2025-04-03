# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(HelloXComponent)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(PREBUILT_CLANG_PATH "/home/penghuang/sources/roms/ohos/prebuilts/clang_linux-x86_64-c5b0b3-20240320")
set(CMAKE_C_COMPILER ${PREBUILT_CLANG_PATH}/bin/clang)
set(CMAKE_CXX_COMPILER ${PREBUILT_CLANG_PATH}/bin/clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DOHOS_PLATFORM)

# add_definitions(-DEGL_EGLEXT_PROTOTYPES=1)
add_definitions(-DNDEBUG=1)

# add_definitions(-DRENDER_EXTRACT_SUPPORTED=1)
add_compile_options(

    # disable rtti
    -fno-rtti

    # disable some warnings
    -Wno-c++11-narrowing
    -Wno-deprecated-declarations
    -Wno-unused-command-line-argument

    # disable some errors
    -Wno-macro-redefined
)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

set(OHOS_ROOT_PATH "/home/penghuang/sources/roms/ohos")
set(FOUNDATION_ROOT_PATH "${OHOS_ROOT_PATH}/foundation")

include_directories(
    ${NATIVERENDER_ROOT_PATH}
    ${NATIVERENDER_ROOT_PATH}/include
)

add_library(surface_control SHARED
    surface_control/LogWrapper.cpp
    surface_control/surface_control.cpp
    surface_control/surface_control.h
    surface_control/surface_transaction.cpp
    surface_control/surface_transaction.h
    surface_control/ndk/surface_control.cpp
    surface_control/ndk/surface_control.h

)

target_include_directories(surface_control PUBLIC
    ${OHOS_ROOT_PATH}
    ${OHOS_ROOT_PATH}/commonlibrary/c_utils
    ${OHOS_ROOT_PATH}/commonlibrary/c_utils/base/include
    ${OHOS_ROOT_PATH}/third_party/skia
    ${OHOS_ROOT_PATH}/third_party/mindspore/mindspore-src/source/third_party/securec/include
    ${FOUNDATION_ROOT_PATH}
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/2d_graphics/include
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/2d_graphics/src
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/2d_graphics/src/drawing/engine_adapter/
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/platform/eventhandler
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/platform/image_native
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/platform/ipc_core
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/platform/utils
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/render_service_base/include
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/render_service_client/core
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/2d_engine/rosen_text/export/rosen_text/
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/interfaces/inner_api/composer
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_2d/rosen/modules/composer/vsync/include
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_surface
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_surface/interfaces/inner_api/common
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_surface/interfaces/inner_api/surface
    ${FOUNDATION_ROOT_PATH}/graphic/graphic_surface/interfaces/inner_api/utils
)

target_link_directories(surface_control PUBLIC
    ${OHOS_ROOT_PATH}/libs
)

target_link_libraries(surface_control PUBLIC
    hilog_ndk.z
    render_service_base.z
    render_service_client.z
    sync_fence.z
)

add_library(helloxcomponent SHARED
    hello/AVPlayer.cpp
    hello/AVPlayer.h
    hello/BitmapRenderer.cpp
    hello/BitmapRenderer.h
    hello/Constants.cpp
    hello/Constants.h
    hello/Compositor.cpp
    hello/Compositor.h
    hello/DelegatedNodeContent.cpp
    hello/DelegatedNodeContent.h
    hello/DisplayManager.cpp
    hello/DisplayManager.h
    hello/GLCore.cpp
    hello/GLCore.h
    hello/GLFence.cpp
    hello/GLFence.h
    hello/GLImage.cpp
    hello/GLImage.h
    hello/GLTexture.cpp
    hello/GLTexture.h
    hello/Log.h
    hello/NapiManager.cpp
    hello/NapiManager.h
    hello/NativeWindow.cpp
    hello/NativeWindow.h
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
    hello/SyncFence.cpp
    hello/SyncFence.h
    napi_init.cpp
)

target_link_libraries(helloxcomponent PUBLIC
    EGL
    GLESv3
    ace_ndk.z
    ace_napi.z
    avplayer
    hilog_ndk.z
    image_source
    native_buffer
    native_display_manager
    native_image
    native_media_core
    native_window
    pixelmap
    uv
    surface_control
)