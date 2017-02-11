// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.


#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl.h"
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <GL/glu.h>


#include "util.hpp"
#include "simple_image.hpp"
#include "field.h"
#include "Camera.hpp"


glm::mat4 MVP;


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
											1280, 720, 
											SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0); // disable vsync, set to 1 to enable. Default: 1
	
	// Setup opengl extension handler
	if (gl3wInit()) { 
		std::cout << "failed to init GL3W" << std::endl;
		clean_up(glcontext, window); 
		return 1;
	}

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);
    ImVec4 clear_color = ImColor(114, 144, 154);

	//------------------------------------------------------------------------------------------------
	//                                     Shader Stuff
	//------------------------------------------------------------------------------------------------

	// program and shader handles
	GLuint shader_program, vertex_shader, fragment_shader;

	// we need these to properly pass the strings
	const char *source;

	printf("About to load shader\n");
	// create and compiler vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source = filetobuf("TransformVertexShader.vertexshader");
	if(source == NULL) printf("Error loading vertex shader file: check path\n");
	glShaderSource(vertex_shader, 1, &source, 0);
	glCompileShader(vertex_shader);
	if (!check_shader_compile_status(vertex_shader)) {
		printf("vertex_shader bad compile status\n");
		clean_up(glcontext, window);
		return 1;
	}


	printf("About to load shader\n");
	// create and compiler fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source =  filetobuf("TextureFragmentShader.fragmentshader");
	if(source == NULL) printf("Error loading fragment shader file: check path\n");
	glShaderSource(fragment_shader, 1, &source, 0);
	glCompileShader(fragment_shader);
	if (!check_shader_compile_status(fragment_shader)) {
		printf("fragment_shader bad compile status\n");
		clean_up(glcontext, window);
		return 1;
	}

	printf("Creating shader_program\n");
	// create program
	shader_program = glCreateProgram();

	// attach shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	// link the program and check for errors
	glLinkProgram(shader_program);
	check_program_link_status(shader_program);
	

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	


	// Get a handle for our "MVP" uniform
	GLuint mvpID = glGetUniformLocation(shader_program, "mvpID");

	// initialise camera
	//setupScene();
	auto camera = new Camera();

	// Load the texture 
	//GLuint texture = LoadTextureRAW( "texture.raw", true );
	GLuint textureID; 
	Image tex_grass("/home/cuan/ClionProjects/temp/comp308_MAC_renderer/work/res/textures/tall-grass.png");
    glGenTextures(1, &textureID); // Generate texture ID

    glBindTexture(GL_TEXTURE_2D, textureID); // Bind it as a 2D texture
    // Finnaly, actually fill the data into our texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex_grass.w, tex_grass.h, tex_grass.glFormat(), GL_UNSIGNED_BYTE,
                      tex_grass.dataPointer());

    //glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex_grass.w,tex_grass.h,0,GL_RGBA,GL_UNSIGNED_BYTE,tex_grass.dataPointer());

    // Setup sampling strategies
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Get a handle for our "myTextureSampler" uniform
	GLuint myTextureSampler  = glGetUniformLocation(shader_program, "myTextureSampler");

	//=======================================================
	
    Field *field = new Field(50);
    
    
	//=======================================================


	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// 1rst attribute buffer : vertices
	GLuint vboVertexID;
	glGenBuffers(1, &vboVertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertexID);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER,
					field->m_points->size() * sizeof(float) * 3,
					field->m_points->data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	GLuint vboUVbufferID;
	glGenBuffers(1, &vboUVbufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vboUVbufferID);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER,
					field->m_uvs->size() * sizeof(float) * 2,
					field->m_uvs->data(),
				    GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size : U+V => 2
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	
	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	
	glBindVertexArray(0);
	glUseProgram(0);






    // Main loop
    bool done = false;
    while (!done)
    {
		
		//setupScene();
		camera->computeMatricesFromInputs();


        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				done = true;
			}

        }


		// Inside loop to be fed from a slider
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 0.0f);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Setup display size (every frame to accommodate for window resizing)
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		glm::mat4 Projection = glm::perspective(glm::radians(75.f), float(width)/float(height), 0.1f, 100.0f);
		glViewport(0, 0, width, height); // TODO probably shouldnt be set every tick
		//glm::mat4 Projection = camera->getProjectionMatrix();

		glm::mat4 View       = camera->getViewMatrix();
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model      = glm::mat4(1.0f);


		// Our ModelViewProjection : multiplication of our 3 matrices
		MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		// Use our shader
		glUseProgram(shader_program);


		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(myTextureSampler, 0);
		

		glBindVertexArray(vaoID);
		


		//glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
		glDrawArrays(GL_TRIANGLES, 0, field->m_points->size()*3);
		

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		glUseProgram(0);

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