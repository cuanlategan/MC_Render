//
// Created by cuan on 12/02/17.
//

#ifndef CGRA_PROJECT_A2_ASSET_FACTORY_H
#define CGRA_PROJECT_A2_ASSET_FACTORY_H

#include <GL/gl3w.h>
#include <GL/gl.h>

class AssetFactory {
private:
    char* filetobuf(char *file);
    bool check_shader_compile_status(GLuint obj);
    bool check_program_link_status(GLuint obj);
public:
    GLint createShaderProgram(char *v_shader_path, char *f_shader_path);
    GLint createTexture(char *tex_path);

    GLint createVAO(int vbo_size, const void *vbo_data, int uv_size, const void *uv_data);
};


#endif //CGRA_PROJECT_A2_ASSET_FACTORY_H
