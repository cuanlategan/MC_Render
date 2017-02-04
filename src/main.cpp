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

// imports to trash
#include "cuan_define.h"
#include "wave_generator.h"
#include "field.h"
#include "basic_shader.h"


// Objects to be rendered
WaveGenerator *g_wave_generator;
Field *field = nullptr;

// Flags
bool draw_grass = true;

// Forward declarations 
void clean_up(SDL_GLContext &, SDL_Window *);

/* A simple function that will read a file into an allocated char pointer buffer */
// FROM: https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)
char* filetobuf(char *file)
{
    FILE *fptr;
    long length;
    char *buf;

    fptr = fopen(file, "rb"); /* Open file for reading */
    if (!fptr) /* Return NULL on failure */
        return NULL;
    fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */

    return buf; /* Return the buffer */
}

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

    /* These pointers will receive the contents of our shader source code files */
    GLchar *vertex_source, *fragment_source;

	// program and shader handles
	GLuint shader_program, vertex_shader, fragment_shader;

	// we need these to properly pass the strings
	const char *source;
	int length;

    /* Read our shaders into the appropriate buffers */
    vertex_source = filetobuf("./res/shaders/tutorial2.vert");
    fragment_source = filetobuf("./res/shaders/tutorial2.frag");
    if(vertex_source == NULL) printf("Error could not load vertex shader file\n");
    if(fragment_source == NULL) printf("Error could not load fragment shader file\n");

	// create and compiler vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source = vertex_source;
	glShaderSource(vertex_shader, 1, &source, 0);
	glCompileShader(vertex_shader);
    if (!check_shader_compile_status(vertex_shader)) {
        printf("Error compiling vertex shader\n");
		clean_up(glcontext, window);
		return 1;
	}

	// create and compiler fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source = fragment_source;
	glShaderSource(fragment_shader, 1, &source, 0);
	glCompileShader(fragment_shader);
	if (!check_shader_compile_status(fragment_shader)) {
		printf("Error compiling fragment shader\n");
        clean_up(glcontext, window);
		return 1;
	}

	// create program
    printf("About to create shader program\n");
	shader_program = glCreateProgram();

	// attach shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	// link the program and check for errors
	glLinkProgram(shader_program);
	check_program_link_status(shader_program);

	// Build grass field 
	g_wave_generator = new WaveGenerator();
    g_wave_generator->addGerstnerWave(13.3, 0.5, 0.8, 1.5, glm::vec2(1.3f, 0.f));
    g_wave_generator->addGerstnerWave(13.3*.77, 0.5*.77, 0.8*.77, 1.5, glm::vec2(1.3f*.77, 0.f));
    g_wave_generator->addGerstnerWave(13.3*1.33, 0.5*1.33, 0.8*1.33, 1.5, glm::vec2(1.3f*1.33, 0.f));
    field = new Field();
    field->generateCluster(GRID_DIMENSION);
    

	//------------------------------------------------------------------------------------------------
	//                                     Initialize Geometry/Material/Lights
	//------------------------------------------------------------------------------------------------

	// obtain location of projection uniform
	GLint View_location = glGetUniformLocation(shader_program, "View");
	GLint Projection_location = glGetUniformLocation(shader_program, "Projection");


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

      
        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
				
		// use the shader program
		glUseProgram(shader_program);
		// calculate ViewProjection matrix
		glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.f);
		// translate the world/view position
		glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom)); //-50.f
		
		// Set the current material (for all objects) to red
        //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        //glColor3f(1.0f, 0.0f, 0.0f); //red
        //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_white_ground_ambient);
        //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_white_ground_specular);
        //glMaterialfv(GL_FRONT, GL_SHININESS, mat_white_ground_shininess);
        //glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_white_ground_diffuse);
        field->renderFieldShader(g_wave_generator, 0.f, shader_program);
		glUseProgram(0); // Will this mess up ImGUI??
		
		
		
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