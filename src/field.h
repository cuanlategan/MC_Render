//
// Created by cuan on 5/31/16.
//

#ifndef FIELD_H
#define FIELD_H
#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

class Field {
    private:
        void addPatch(float x, float y, float z);

    public:
        std::vector<glm::vec3>  *m_points;
        std::vector<glm::vec2> *m_uvs;
        std::vector<glm::vec3> *positions;

        Field();
        Field(int num_clusters);






};


#endif //FIELD_H
