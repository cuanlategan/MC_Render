//
// Created by cuan on 12/02/17.
//
#ifndef CGRA_PROJECT_A2_ASSET_FACTORY_CPP
#define CGRA_PROJECT_A2_ASSET_FACTORY_CPP

#include "asset_factory.h"
#include <GL/glu.h>
#include <iostream>

#include "simple_image.hpp"





/* A simple function that will read a file into an allocated char pointer buffer */
char* AssetFactory::filetobuf(char *file)
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

//------------------------------------------------------------------------------------------------
//                                     Shader Functions
//------------------------------------------------------------------------------------------------
GLint AssetFactory::createShaderProgram(char *v_shader_path , char *f_shader_path){

    // program and shader handles
    GLuint shader_program, vertex_shader, fragment_shader;

    // we need these to properly pass the strings
    const char *source;

    printf("About to load shader\n");
    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = filetobuf(v_shader_path);
    if(source == NULL) printf("Error loading vertex shader file: check path\n");
    glShaderSource(vertex_shader, 1, &source, 0);
    glCompileShader(vertex_shader);
    if (!check_shader_compile_status(vertex_shader)) {
        printf("vertex_shader bad compile status\n");
        return 0;
    }


    printf("About to load shader\n");
    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source =  filetobuf(f_shader_path);
    if(source == NULL) printf("Error loading fragment shader file: check path\n");
    glShaderSource(fragment_shader, 1, &source, 0);
    glCompileShader(fragment_shader);
    if (!check_shader_compile_status(fragment_shader)) {
        printf("fragment_shader bad compile status\n");
        return 0;
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
    GLint mvpID = glGetUniformLocation(shader_program, "mvpID");

    return shader_program;
}


// helper to check and display for shader compiler errors
bool AssetFactory::check_shader_compile_status(GLuint obj) {
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
bool AssetFactory::check_program_link_status(GLuint obj) {
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


//------------------------------------------------------------------------------------------------
//                                     Texture Functions
//------------------------------------------------------------------------------------------------
GLint AssetFactory::createTexture(char *tex_path){
    // Load the texture
    GLuint textureID;
    Image tex(tex_path);
    glGenTextures(1, &textureID); // Generate texture ID

    glBindTexture(GL_TEXTURE_2D, textureID); // Bind it as a 2D texture
    // Finnaly, actually fill the data into our texture
    /*gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex.w, tex.h, tex.glFormat(), GL_UNSIGNED_BYTE,
                      tex.dataPointer());*/

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tex.w,tex.h,0,GL_RGBA,GL_UNSIGNED_BYTE,tex.dataPointer());

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    //glGenerateMipmap(GL_TEXTURE_2D);

    // Setup sampling strategies
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return textureID;

}


GLint AssetFactory::createVAO(int vbo_size, const void* vbo_data, int uv_size, const void* uv_data ){
    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // 1rst attribute buffer : vertices
    GLuint vboVertexID;
    glGenBuffers(1, &vboVertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertexID);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,
                 vbo_size,
                 vbo_data, GL_DYNAMIC_DRAW);

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
                 uv_size,
                 uv_data,
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

    return vaoID;
}

#endif //CGRA_PROJECT_A2_ASSET_FACTORY_CPP