cmake_minimum_required (VERSION 3.18.1)
project ("ef")

# DEFINITIONS
add_compile_definitions(EF_PLATFORM_WINDOWS EF_BUILD_DLL)
set(EF_PLATFORM_WINDOWS 0)

find_package(png16 REQUIRED CONFIG)
find_package(game-activity REQUIRED CONFIG)
find_package(games-controller REQUIRED CONFIG)

set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/libs/glad)
set(GLM_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party/glm/glm)
message(STATUS "GLM_DIR dir: ${GLM_DIR}")
add_subdirectory(${GLM_DIR})
# VARS
file(GLOB_RECURSE SOURCES
		${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
		${CMAKE_CURRENT_LIST_DIR}/src/*.c
		${PROJECT_SOURCE_DIR}/src/*.c
	#${GLAD_DIR}/src/glad.c
)

file(GLOB_RECURSE INCLUDE_DIRS
    ${GLAD_DIR}/include
)

# put generated files into the same directory as exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(EF_PLATFORM_WINDOWS)
	add_library(ef SHARED
			${SOURCES}

			)
else()
	add_library(ef SHARED

			${SOURCES}

			)
endif()

#target_link_libraries(ef efAndroid)
##add_library(ef SHARED IMPORTED )
#
#
##link_directories(${CMAKE_CURRENT_LIST_DIR}/src/main/jniLibs/${ANDROID_ABI})
#
#
#target_include_directories(${PROJECT_NAME} PUBLIC
#	#${INCLUDE_DIRS}
#	${SFML_DIR}/include
#    ${GLAD_DIR}/include
#)
#
#
#target_link_directories(
#		${PROJECT_NAME} PUBLIC
#		${SDL_LIB_DIR}
#
#)
#target_link_libraries(
#		${PROJECT_NAME}
#		#SDL2main
#		#SDL2
#		${CMAKE_CURRENT_LIST_DIR}/src/main/jniLibs/${ANDROID_ABI}/libSDL2.so
#
#	#${log-lib}
#
#)
#
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#	#COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL_LIB_DIR}/SDL2.dll ${CMAKE_CURRENT_LIST_DIR}/out/build/x64-Debug/
#	COMMENT "Copying dlls..."
#)

#if(EF_PLATFORM_WINDOWS)
#	set(SDL_LIB_DIR ${SFML_DIR}/lib/windows)
#else()
#	set(SDL_LIB_DIR ${SFML_DIR}/lib/android/arm64-v8a)
#endif()

#set(SDL_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/src/main/jniLibs/${ANDROID_ABI})
# GLAD

# Export GameActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
		"${CMAKE_SHARED_LINKER_FLAGS} -u GameActivity_onCreate")

# Set common compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
add_definitions("-DIMGUI_IMPL_OPENGL_ES2")

# source files common to both the GameActivity and NativeActivity versions
set(COMMON_SRC_DIR "platform/android/src/common")
set(CPP_SRC_DIR "platform/android/src/main/cpp")

# common include directory for all samples
set(COMMON_INCLUDE_DIR "platform/android/src/common/include")

target_include_directories(ef PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${COMMON_SRC_DIR}
		${COMMON_INCLUDE_DIR}
		${IMGUI_BASE_DIR}
		${CPP_SRC_DIR}
		${GLM_DIR}
		)

target_compile_options(ef
		PRIVATE
		-std=c++17
		-Wall
		-Wextra-semi
		-Wshadow
		-Wshadow-field
		-O0
		#"$<$<CONFIG:DEBUG>:-Werror>"
        )

# add lib dependencies
target_link_libraries(ef
		android
		game-activity::game-activity
		games-controller::paddleboat_static
		png16::png16-static
		atomic
		EGL
		GLESv2
		jnigraphics
		log
		z
		glm)
