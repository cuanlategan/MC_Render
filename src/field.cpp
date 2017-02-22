#include <GL/gl3w.h>
#include <GL/gl.h>
#include "field.h"

using namespace std;
using namespace glm;

Field::Field(){
    m_points = new vector<vec3>();
    m_uvs = new vector<vec2>();
    positions = new vector<vec3>();
}

Field::Field(int num_clusters): Field() {

    for (int i = 0; i < num_clusters; i++) {
        for (int j = 0; j < num_clusters; j++) {

            float x = float(i);
            float z = float(j);
            float y = 0.f;
            
            addPatch(x,y,z);
        }
    }

}

void Field::attachAssetIDs(GLuint shader, GLuint tex, GLuint tex_samp, GLuint vao, GLuint uni) {
    shader_id = shader;
    tex_id = tex;
    tex_sampler = tex_samp;
    vao_id = vao;
    uni_id = uni;
}


void Field::render(glm::mat4 view_project){

    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP =  view_project * model; // Remember, matrix multiplication is the other way around

    // Use our shader
    glUseProgram(shader_id);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(uni_id, 1, GL_FALSE, &MVP[0][0]);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(tex_sampler, 0);

    glUniform1f(glGetUniformLocation(shader_id, "time"), (time+=.002f)); // set time uniform in shaders
    glUniform1f(glGetUniformLocation(shader_id, "wavelength"), wavelength);
    glUniform1f(glGetUniformLocation(shader_id, "amplitude"), amplitude);
    glUniform1f(glGetUniformLocation(shader_id, "steepnes"), steepnes);
    glUniform1f(glGetUniformLocation(shader_id, "speed"), speed);
    glUniform1i(glGetUniformLocation(shader_id, "hasTex"), 1);
    glUniform1f(glGetUniformLocation(shader_id, "dirX"), dirX);
    glUniform1f(glGetUniformLocation(shader_id, "dirY"), dirY);

    glBindVertexArray(vao_id);

    glDrawArrays(GL_TRIANGLES, 0, m_points->size()*3);

    glUseProgram(0);

}


void Field::addPatch(float x, float y, float z){

    const float height = 0.8f;
    const float half_width = 0.3f;

    const vector<vec3> blade_vert = {
        vec3( -half_width, 0.f,    0.f ),
        vec3( half_width,  0.f,    0.f ),
        vec3( half_width,  height, 0.f ),
        vec3( -half_width, 0.f,    0.f ),
        vec3( half_width,  height, 0.f ),
        vec3( -half_width, height, 0.f )
    };

    const vector<vec2> blade_uv = {
        vec2( 1.f,1.f ),
        vec2( 0.f,1.f ),
        vec2( 0.f,0.f ),
        vec2( 1.f,1.f ),
        vec2( 0.f,0.f ),
        vec2( 1.f,0.f )
    };

    positions->push_back(vec3(x,y,z));
    
    for(const auto &v: blade_vert){
        m_points->push_back(vec3(v.x+x, v.y+y, v.z+z));
    }
    for(const auto &v: blade_vert){
        vec3 tmp = rotateY(v,radians(45.f));
        m_points->push_back(vec3(tmp.x+x,tmp.y+y,tmp.z+z));
    }
    for(const auto &v: blade_vert){
        vec3 tmp = rotateY(v,radians(-45.f));
        m_points->push_back(vec3(tmp.x+x,tmp.y+y,tmp.z+z));
    }

    for(const auto &v: blade_uv){
        m_uvs->push_back(v);
    }
    for(const auto &v: blade_uv){
        m_uvs->push_back(v);
    }
    for(const auto &v: blade_uv){
        m_uvs->push_back(v);
    }

    
}




