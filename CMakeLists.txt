# CMake build file
# based on https://github.com/Pesc0/imgui-cmake and https://gist.github.com/rokups/f771217b2d530d170db5cb1e08e9a8f4

cmake_minimum_required(VERSION 3.2)

project(ImageInside LANGUAGES C CXX)

option(IMGUI_IMPL_DX12 "Build the DirectX 12 implementation (only if supported)" ${WIN32})

set(IMGUI_DIR "imgui")
add_library(IMGUI STATIC)

if (IMGUI_IMPL_DX12)
        target_sources(IMGUI PRIVATE
            ${IMGUI_DIR}/imgui_draw.cpp
            ${IMGUI_DIR}/imgui_tables.cpp
            ${IMGUI_DIR}/imgui_widgets.cpp
            ${IMGUI_DIR}/imgui.cpp

            ${IMGUI_DIR}/backends/imgui_impl_win32.h
            ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
            ${IMGUI_DIR}/backends/imgui_impl_dx12.h
            ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
        )
        target_link_libraries(IMGUI INTERFACE d3d12 dxgi)
endif()

target_include_directories( IMGUI
                            PUBLIC ${IMGUI_DIR}
                            )

add_executable(ImageInside ImageInside.cpp Correlation.cpp)

target_compile_features(ImageInside PUBLIC cxx_std_11)

target_sources(ImageInside PRIVATE ImageInside.cpp)
target_include_directories(ImageInside PUBLIC .)

target_link_libraries (ImageInside PUBLIC IMGUI)