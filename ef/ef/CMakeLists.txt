cmake_minimum_required (VERSION 3.18.1)
project ("ef")



add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/platform/android/src/main/cpp)
# DEFINITIONS
add_compile_definitions(EF_PLATFORM_WINDOWS EF_BUILD_DLL)
set(EF_PLATFORM_WINDOWS 0)


set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/libs/glad)
# VARS
file(GLOB_RECURSE SOURCES
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/src/*.c
	${GLAD_DIR}/src/glad.c
)
file(GLOB_RECURSE INCLUDE_DIRS
    ${GLAD_DIR}/include
)
# put generated files into the same directory as exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(ef SHARED ${SOURCES})
target_link_libraries(ef game)
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