# CMake build file
# based on https://github.com/Pesc0/imgui-cmake and https://gist.github.com/rokups/f771217b2d530d170db5cb1e08e9a8f4

cmake_minimum_required(VERSION 3.2)

project(ImageInside LANGUAGES C CXX)

option(IMGUI_IMPL_GLFW "Build the GLFW implementation (only if supported)" ON)

set(IMGUI_DIR "imgui")
add_library(IMGUI STATIC)


if (IMGUI_IMPL_GLFW)
	find_package(OpenGL REQUIRED)
	
	# PkgConfig will be needed for SDL and glfw on some platforms.
	find_package(PkgConfig QUIET)	

	if (MSVC)
		# Compiled library is shipped with the repository for Visual Studio.
		add_library(glfw INTERFACE IMPORTED)
		target_link_libraries(glfw INTERFACE ../imgui/backends/GLFW/glfw3 opengl32)
		target_include_directories(glfw INTERFACE ${IMGUI_DIR}/backends/GLFW/)
		target_sources(IMGUI PRIVATE ${IMGUI_DIR}/backends/GLFW/glfw3.lib)
	elseif (PkgConfig_FOUND)
		pkg_check_modules(GLFW QUIET glfw3)
		if (GLFW_FOUND)
			add_library(glfw INTERFACE IMPORTED)
			target_link_libraries(glfw INTERFACE ${GLFW_LDFLAGS})
			target_compile_options(glfw INTERFACE ${GLFW_CFLAGS})
		endif ()
	endif ()

	target_sources(IMGUI PRIVATE
		${IMGUI_DIR}/imgui_draw.cpp
		${IMGUI_DIR}/imgui_tables.cpp
		${IMGUI_DIR}/imgui_widgets.cpp
		${IMGUI_DIR}/imgui.cpp

		${IMGUI_DIR}/backends/imgui_impl_glfw.h
		${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
		${IMGUI_DIR}/backends/imgui_impl_opengl3.h
		${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
		${IMGUI_DIR}/backends/GL/gl3w.c
		${IMGUI_DIR}/backends/GL/gl3w.h
		${IMGUI_DIR}/backends/GL/glcorearb.h
		${IMGUI_DIR}/backends/GLFW/glfw3.h
		${IMGUI_DIR}/backends/GLFW/glfw3native.h
	)
	target_link_libraries(IMGUI INTERFACE glfw)
endif()

target_include_directories( IMGUI
                            PUBLIC ${IMGUI_DIR}
                            )

add_executable(ImageInside ImageInside.cpp)

target_compile_features(ImageInside PUBLIC cxx_std_17)

target_sources(ImageInside PRIVATE ImageInside.cpp Settings.cpp Settings.h Correlation.cpp Correlation.h CandidateFinder.cpp CandidateFinder.h CandidateSettings.h
                                   Candidate.h LoadTexture.cpp LoadTexture.h ImFileDialog/ImFileDialog.cpp ImFileDialog/ImFileDialog.h stb_image/stb_image.h test.png)

target_include_directories(ImageInside PUBLIC .)

target_link_libraries (ImageInside PUBLIC IMGUI glfw)