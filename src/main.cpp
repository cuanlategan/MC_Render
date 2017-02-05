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

#include <GL/glu.h>

#include "simple_image.hpp"

// Projection values
//
float g_fovy = 60.0;
float g_znear = 0.005;
float g_zfar = 100.0;

glm::vec3 g_camera_eye(0.f, 0.f, 0.f);

// Mouse controlled Camera values
//
bool g_leftMouseDown = false;
glm::vec2 g_mousePosition;
float g_pitch = 0;
float g_yaw = 0;
float g_zoom = 1.0;

glm::mat4 MVP;

void setupCamera(){
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,2,1.5), // Camera is at (0,2,1.5), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f); 
	Model = glm::rotate(Model, g_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, g_yaw, glm::vec3(0.f, 1.f, 0.f));
	Model = glm::translate(Model, glm::vec3(-g_camera_eye.x, -g_camera_eye.y, -g_camera_eye.z));
	
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// View = glm::rotate(View, 30.0f*std::sin(0.1f*t), glm::vec3(1.0f, 0.0f, 0.0f));
}

// Clean up helper
void clean_up(SDL_GLContext &, SDL_Window *);

// http://www.nullterminator.net/gltexture.html
// load a 256x256 RGB .RAW file as a texture
GLuint LoadTextureRAW( const char * filename, int wrap )
{
  GLuint texture;
  int width, height;
  char * data;
  FILE * file;

  // open texture data
  file = fopen( filename, "rb" );
  if ( file == NULL ) return 0;

  // allocate buffer
  width = 256;
  height = 256;
  data = (char*)malloc( width * height * 3 );

  // read texture data
  fread( data, width * height * 3, 1, file );
  fclose( file );

  // allocate a texture name
  glGenTextures( 1, &texture );

  // select our current texture
  glBindTexture( GL_TEXTURE_2D, texture );

  
  // when texture area is small, bilinear filter the closest MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_LINEAR_MIPMAP_NEAREST );
  // when texture area is large, bilinear filter the first MIP map
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  // if wrap is true, the texture wraps over at the edges (repeat)
  //       ... false, the texture ends at the edges (clamp)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                   wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                   wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE );

  // build our texture MIP maps
  gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,
    height, GL_RGB, GL_UNSIGNED_BYTE, data );

  // free buffer
  free( data );

  return texture;

}

/* A simple function that will read a file into an allocated char pointer buffer */
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
		std::cerr << "check_shader_compile_status error: "<< &log[0];
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
		std::cerr << "check_program_link_status error: " << &log[0];
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
	setupCamera();

	// Load the texture 
	//GLuint texture = LoadTextureRAW( "texture.raw", true );
	GLuint textureID; 
	Image tex_grass("/home/cuan/ClionProjects/temp/comp308_MAC_renderer/work/res/textures/brick.jpg");
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

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	/*glm::vec3 p(0.f, 0.f, 0.f);
	float height = 0.015f;
    float half_width = 0.0075f;
    float r_half_width = 0.005303f;
	*/
	static const GLfloat g_vertex_buffer_data[] = { 
	/*half_width+p.x, 0.f+p.y, height+p.z,
    -half_width+p.x, 0.f+p.y, height+p.z,
    half_width+p.x, 0.f+p.y, 0.f+p.z,
    //bottom face
    -half_width+p.x, 0.f+p.y, 0.f+p.z,
    half_width+p.x, 0.f+p.y, 0.f+p.z,
    -half_width+p.x, 0.f+p.y, height+p.z,
	
	// blade 2
    r_half_width+p.x, -r_half_width+p.y, height+p.z,
    -r_half_width+p.x, r_half_width+p.y, height+p.z,
    r_half_width+p.x, -r_half_width+p.y, 0.f+p.z,
	//bottom face
    -r_half_width+p.x, r_half_width+p.y, 0.f+p.z,
    r_half_width+p.x, -r_half_width+p.y, 0.f+p.z,
    -r_half_width+p.x, r_half_width+p.y, height+p.z,

	// blade 3
    r_half_width+p.x, r_half_width+p.y, height+p.z,
    -r_half_width+p.x, -r_half_width+p.y, height+p.z,
    r_half_width+p.x, r_half_width+p.y, 0.f+p.z,
	 //bottom face
	-r_half_width+p.x, -r_half_width+p.y, 0.f+p.z,
	r_half_width+p.x, r_half_width+p.y, 0.f+p.z,
	-r_half_width+p.x, -r_half_width+p.y, height+p.z
	*/
	
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f

	};

	// Two UV coordinatesfor each vertex. They were created with Blender.
	static const GLfloat g_uv_buffer_data[] = { 
		/*
		1.f,0.f,
    	0.f,0.f,
    	1.f,1.f,
		//bottom face
    	0.f,1.f,
    	1.f,1.f,
    	0.f,0.f,
		// blade 2
    	1.f,0.f,
    	0.f,0.f,
    	1.f,1.f,
		//bottom face
    	0.f,1.f,
    	1.f,1.f,
    	0.f,0.f,
    	// blade 3
    	1.f,0.f,
    	0.f,0.f,
    	1.f,1.f,
    	//bottom face
    	0.f,1.f,
    	1.f,1.f,
    	0.f,0.f
		*/

		
		0.000059f, 1.0f-0.000004f, 
		0.000103f, 1.0f-0.336048f, 
		0.335973f, 1.0f-0.335903f, 
		1.000023f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.336024f, 1.0f-0.671877f, 
		0.667969f, 1.0f-0.671889f, 
		1.000023f, 1.0f-0.000013f, 
		0.668104f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.000059f, 1.0f-0.000004f, 
		0.335973f, 1.0f-0.335903f, 
		0.336098f, 1.0f-0.000071f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.336024f, 1.0f-0.671877f, 
		1.000004f, 1.0f-0.671847f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.668104f, 1.0f-0.000013f, 
		0.335973f, 1.0f-0.335903f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.668104f, 1.0f-0.000013f, 
		0.336098f, 1.0f-0.000071f, 
		0.000103f, 1.0f-0.336048f, 
		0.000004f, 1.0f-0.671870f, 
		0.336024f, 1.0f-0.671877f, 
		0.000103f, 1.0f-0.336048f, 
		0.336024f, 1.0f-0.671877f, 
		0.335973f, 1.0f-0.335903f, 
		0.667969f, 1.0f-0.671889f, 
		1.000004f, 1.0f-0.671847f, 
		0.667979f, 1.0f-0.335851f
		
	};
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// 1rst attribute buffer : vertices
	GLuint vboVertexID;
	glGenBuffers(1, &vboVertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertexID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
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
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
    bool done = false;
	GLfloat zoom = -50.f;
    while (!done)
    {
		
		setupCamera();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
			if (event.type == SDL_QUIT) {
				done = true;
				break;
			}
            
			else if (event.type == SDL_KEYDOWN) {
				float const scale = 0.5f;			
				if (keys[SDL_SCANCODE_A]) {
					g_camera_eye.x -= cos(g_yaw) * scale;
        			g_camera_eye.z -= sin(g_yaw) * scale;
					break;
				}
				if (keys[SDL_SCANCODE_B]) {
					std::cout << "B is being pressed\n";
					break;
				}
				if (keys[SDL_SCANCODE_D]) {
					g_camera_eye.x += float(cos(g_yaw)) * scale;
        			g_camera_eye.z += float(sin(g_yaw)) * scale;
					break;
				}
				if (keys[SDL_SCANCODE_W]) {
					g_camera_eye.x += sin(g_yaw) * scale;
        			g_camera_eye.z -= cos(g_yaw) * scale;
        			g_camera_eye.y -= sin(g_pitch) * scale;
					break;
				}
				if (keys[SDL_SCANCODE_S]) {
					g_camera_eye.x -= sin(g_yaw) * scale;
        			g_camera_eye.z += cos(g_yaw) * scale;
        			g_camera_eye.y += sin(g_pitch) * scale;
					break;
				}
			}
			
			else if (event.type == SDL_MOUSEBUTTONDOWN){
				if(event.button.button == SDL_BUTTON_LEFT){
					//SDL_ShowSimpleMessageBox(0, "Mouse", "Left button was pressed!", window);	
					break;
				}
				
			}
		
			
        }
		/*
		SDL_Event eventTO;
		while(SDL_WaitEventTimeout(&eventTO, 32)){
			if (eventTO.type == SDL_MOUSEMOTION){
				int diffX = g_mousePosition.x - eventTO.motion.x;
    			int diffY = g_mousePosition.y - eventTO.motion.y;

				g_yaw -= ( diffX)*0.01f;
        		g_pitch -= ( diffY)*0.01f; 

				g_mousePosition = glm::vec2(eventTO.motion.x, eventTO.motion.y);

				//std::cout << "mouseX: " << mouseX
				//		  <<  " MouseY: " << mouseY << "\n";
				break;
			}
		}
		*/

		// Inside loop to be fed from a slider
		glClearColor(clear_color.x, clear_color.y, clear_color.z, 0.0f);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		
		// Use our shader
		glUseProgram(shader_program);
		//glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(myTextureSampler, 0);
		
	
		//glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);
		glBindVertexArray(vaoID);
		

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

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