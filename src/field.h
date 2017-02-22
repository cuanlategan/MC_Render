#ifndef FIELD_H
#define FIELD_H
#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class Field {
    private:
    GLuint shader_id, tex_id, tex_sampler, vao_id, uni_id;
    glm::mat4 model = glm::mat4(1.f);
    void addPatch(float x, float y, float z);


    float wavelength = 0.5f;
    float amplitude = 0.05f;
    float steepnes = 0.8f;
    float speed = 0.05f;
    int hasTex = 1;
    float dirX = 1.3f;
    float dirY = 0.0f;
    float time = 0.f;


    public:
    std::vector<glm::vec3>  *m_points;
    std::vector<glm::vec2> *m_uvs;
    std::vector<glm::vec3> *positions;

    Field();
    Field(int num_clusters);

    void render(glm::mat4 view_project);


    void attachAssetIDs(GLuint shader, GLuint tex, GLuint tex_samp, GLuint vao, GLuint uni);
};

#endif //FIELD_H
