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

#include "basic_shader.h"

// Clean up helper
void clean_up(SDL_GLContext &, SDL_Window *);

// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj) {
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj) {
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}


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
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	// Setup opengl extension wrangler
	if (gl3wInit()) { 
		std::cout << "failed to init GL3W" << std::endl;
		clean_up(glcontext, window); 
		return 1;
	}

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);
	  
    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

	//------------------------------------------------------------------------------------------------
	//                                     Shader Stuff
	//------------------------------------------------------------------------------------------------

	// program and shader handles
	GLuint shader_program, vertex_shader, geometry_shader, fragment_shader;

	// we need these to properly pass the strings
	const char *source;
	int length;

	// create and compiler vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source = vertex_source.c_str();
	length = vertex_source.size();
	glShaderSource(vertex_shader, 1, &source, &length);
	glCompileShader(vertex_shader);
	if (!check_shader_compile_status(vertex_shader)) {
		clean_up(glcontext, window);
		return 1;
	}

	// create and compiler geometry shader
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	source = geometry_source.c_str();
	length = geometry_source.size();
	glShaderSource(geometry_shader, 1, &source, &length );
	glCompileShader(geometry_shader);
	if (!check_shader_compile_status(geometry_shader)) {
		clean_up(glcontext, window);
		return 1;
	}

	// create and compiler fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source = fragment_source.c_str();
	length = fragment_source.size();
	glShaderSource(fragment_shader, 1, &source, &length);
	glCompileShader(fragment_shader);
	if (!check_shader_compile_status(fragment_shader)) {
		clean_up(glcontext, window);
		return 1;
	}

	// create program
	shader_program = glCreateProgram();

	// attach shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);

	// link the program and check for errors
	glLinkProgram(shader_program);
	check_program_link_status(shader_program);

	//------------------------------------------------------------------------------------------------
	//                                     Initialize Geometry/Material/Lights
	//------------------------------------------------------------------------------------------------

	// obtain location of projection uniform
	GLint View_location = glGetUniformLocation(shader_program, "View");
	GLint Projection_location = glGetUniformLocation(shader_program, "Projection");

	// vao and vbo handle
	GLuint vao, vbo;

	// generate and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// generate and bind the vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// create a galaxylike distribution of points
	const int particles = 128 * 1024;
	std::vector<GLfloat> vertexData(particles * 3);
	for (int i = 0; i<particles; ++i)
	{
		int arm = 3 * (std::rand() / float(RAND_MAX));
		float alpha = 1 / (0.1f + std::pow(std::rand() / float(RAND_MAX), 0.7f)) - 1 / 1.1f;
		float r = 4.0f*alpha;
		alpha += arm*2.0f*3.1416f / 3.0f;

		vertexData[3 * i + 0] = r*std::sin(alpha);
		vertexData[3 * i + 1] = 0;
		vertexData[3 * i + 2] = r*std::cos(alpha);

		vertexData[3 * i + 0] += (4.0f - 0.2*alpha)*(2 - (std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX) +
			std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX)));
		vertexData[3 * i + 1] += (2.0f - 0.1*alpha)*(2 - (std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX) +
			std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX)));
		vertexData[3 * i + 2] += (4.0f - 0.2*alpha)*(2 - (std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX) +
			std::rand() / float(RAND_MAX) + std::rand() / float(RAND_MAX)));
	}

	// fill with data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexData.size(), &vertexData[0], GL_STATIC_DRAW);


	// set up generic attrib pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));

	// we are blending so no depth testing
	glDisable(GL_DEPTH_TEST);

	// enable blending
	glEnable(GL_BLEND);
	//  and set the blend function to result = 1*source + 1*destination
	glBlendFunc(GL_ONE, GL_ONE);

    // Main loop
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
    bool done = false;
	GLfloat zoom = -50.f;
    while (!done)
    {
		

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				done = true;
			}
            
			if (event.type == SDL_KEYDOWN) {
							
				if (keys[SDL_SCANCODE_A]) {
					std::cout << "A is being pressed\n";
					show_another_window = !show_another_window;
				}
				if (keys[SDL_SCANCODE_B]) {
					std::cout << "B is being pressed\n";
				}
				if (keys[SDL_SCANCODE_W]) {
					zoom += 1.f;
				}
				if (keys[SDL_SCANCODE_S]) {
					zoom -= 1.f;
				}
			
			}
			
        }
        ImGui_ImplSdlGL3_NewFrame(window);

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		static float speed = 0.5f;
		{
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &speed, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }



        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
		// get the time in ticks
		float t = SDL_GetTicks() / 1000.f;
		t *= speed;
		
		// use the shader program
		glUseProgram(shader_program);
		// calculate ViewProjection matrix
		glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.f);
		// translate the world/view position
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom)); //-50.f
		// make the camera rotate around the origin
		View = glm::rotate(View, 30.0f*std::sin(0.1f*t), glm::vec3(1.0f, 0.0f, 0.0f));
		View = glm::rotate(View, -22.5f*t, glm::vec3(0.0f, 1.0f, 0.0f));
		// set the uniform
		glUniformMatrix4fv(View_location, 1, GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(Projection_location, 1, GL_FALSE, glm::value_ptr(Projection));
		// bind the vao
		glBindVertexArray(vao);
		// draw
		glDrawArrays(GL_POINTS, 0, particles);
		
		
		
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