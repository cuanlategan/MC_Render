// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.


#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl.h"
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>


#include "asset_factory.h"
#include "field.h"
#include "camera.h"


static const int G_WINDOW_W = 1280;
static const int G_WINDOW_H = 720;
glm::mat4 MVP;
AssetFactory asset_factory;

void clean_up(SDL_GLContext& glcontext, SDL_Window* window);


int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
		
    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL3 example",
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  G_WINDOW_W, G_WINDOW_H,
											SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // disable vsync, set to 1 to enable. Default: 1
	
	// Setup opengl extension handler
	if (gl3wInit()) { 
		std::cout << "failed to init GL3W" << std::endl;
		clean_up(glcontext, window); 
		return 1;
	}

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);
    ImVec4 clear_color = ImColor(114, 144, 154);


	auto field = new Field(50);

	GLint shader_program = asset_factory.createShaderProgram("basic_gerst_vert.glsl",
															 "TextureFragmentShader.fragmentshader");


/*
	GLint shader_program = asset_factory.createShaderProgram("TransformVertexShader.vertexshader",
															   "TextureFragmentShader.fragmentshader");
*/

	GLint textureID = asset_factory.createTexture("/home/cuan/ClionProjects/temp/comp308_MAC_renderer/work/res/textures/tall-grass.png");

	// Get handles for our uniforms
	GLint mvpID = glGetUniformLocation(shader_program, "mvpID");
	GLint myTextureSampler  = glGetUniformLocation(shader_program, "myTextureSampler");


	GLint vaoID = asset_factory.createVAO(field->m_points->size() * sizeof(float) *3,
										  field->m_points->data(),
										  field->m_uvs->size() * sizeof(float) *2,
										  field->m_uvs->data());


	field->attachAssetIDs(shader_program, textureID, myTextureSampler, vaoID, mvpID);
	// initialise camera
	auto camera = new Camera();



    // Main loop
    bool done = false;
    while (!done)
    {

		// Let the camera look for keyboard changes
		camera->computeMatricesFromInputs();

		int width = G_WINDOW_W;
		int height = G_WINDOW_H;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				done = true;
			}
			else if(event.type == SDL_WINDOWEVENT){
				// Setup display size to accommodate for window resizing
				if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
					width = event.window.data1;
					height = event.window.data2;
					glViewport(0, 0, width, height);
				}

			}


        }


		// Inside loop to be fed from a slider
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 0.0f);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 Projection = glm::perspective(glm::radians(75.f), float(width)/float(height), 0.1f, 100.0f);
		glm::mat4 View       = camera->getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		//glm::mat4 Model      = glm::mat4(1.0f);
		//MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		field->render(Projection*View);



        ImGui_ImplSdlGL3_NewFrame(window);

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		static float speed = 0.5f;
		{
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &speed, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }


		ImGui::Render();

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
	clean_up(glcontext, window);
    

    return 0;
}


void clean_up(SDL_GLContext& glcontext, SDL_Window* window) {
	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}