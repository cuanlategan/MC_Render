
cmake_minimum_required(VERSION 2.8)

project(GLEW C)

# enable IDE project folders
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)

SET(headers
	"include/GL/gl3w.h"
	"include/GL/glcorearb.h"
)

SET(sources
	"src/gl3w.c"
)

# glew lib target
add_library(gl3w ${headers} ${sources})
set_property(TARGET gl3w PROPERTY FOLDER "GL3W")

# include dir usage requirements
target_include_directories(gl3w PUBLIC "${PROJECT_SOURCE_DIR}/include")

# library usage requirements
target_link_libraries(gl3w PRIVATE ${OPENGL_LIBRARIES})

# system-specific library usage requirements
if(WIN32)
	target_link_libraries(gl3w PRIVATE kernel32)
endif()

# usage requirements as shared/static lib
if(BUILD_SHARED_LIBS)
	# define macro for building DLL
	target_compile_definitions(gl3w PRIVATE GLEW_BUILD)
else()
	# define macro for static linking
	target_compile_definitions(gl3w PUBLIC GLEW_STATIC)
endif()


