//
// Created by cuan on 5/26/16.
//

#ifndef CGRA_PROJECT_A3_GRASS_H
#define CGRA_PROJECT_A3_GRASS_H
#include <vector>
#include <GL/gl3w.h>
#include <GL/glu.h>
#include <glm/glm.hpp>


class Grass {
private:
    std::vector<glm::vec3> m_points;	// Point list
    std::vector<glm::vec2> m_uvs;
    glm::vec3 position;

    GLfloat m_shear[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };

public:
    Grass();
    Grass(glm::vec3 position);
    void renderGeometry();
    const glm::vec3& getPosition(){return position;}

    std::vector<glm::vec3> getPoints(){ return m_points;}
    std::vector<glm::vec2> getUvs(){ return m_uvs;}

    void setShear(const glm::vec3 &shear);
};


#endif //CGRA_PROJECT_A3_GRASS_H
