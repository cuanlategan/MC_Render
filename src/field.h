#ifndef FIELD_H
#define FIELD_H
#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class Field {
    private:
    GLint shader_id, tex_id, tex_sampler, vao_id, uni_id;
    glm::mat4 model = glm::mat4(1.f);
    void addPatch(float x, float y, float z);
    public:
        std::vector<glm::vec3>  *m_points;
        std::vector<glm::vec2> *m_uvs;
        std::vector<glm::vec3> *positions;

        Field();
        Field(int num_clusters);

        void attachAssetIDs(GLint shader_id, GLint tex_id, GLint tex_sampler,
                            GLuint vba_id,   GLint uni_idi);
        void render(glm::mat4 view_project);






};

#endif //FIELD_H
