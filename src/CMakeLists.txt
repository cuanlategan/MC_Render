# If you add new source files, you must list them here
#

# TODO list your header files (.hpp) here
SET(headers
	"imgui_impl.h"
	"simple_image.hpp"
	"asset_factory.h"
	"field.h"
	"Camera.hpp"
	)


# TODO list your source files (.cpp) here
SET(sources
	"main.cpp"
	"imgui_impl.cpp"
	"asset_factory.cpp"
	"field.cpp"
	"Camera.cpp"
	)



# Add executable target and link libraries
# You do not need to touch this
add_executable(${CGRA_PROJECT} ${headers} ${sources})
target_link_libraries(${CGRA_PROJECT} PRIVATE gl3w )
target_link_libraries(${CGRA_PROJECT} PRIVATE imgui)

target_link_libraries(${CGRA_PROJECT} PRIVATE SDL2)
target_link_libraries(${CGRA_PROJECT} PRIVATE SDL2main)
target_link_libraries(${CGRA_PROJECT} PRIVATE stb)