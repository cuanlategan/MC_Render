#include <vector>
#include "grass.h"


using namespace std;


void Grass::setShear(const glm::vec3& shear) {
    m_shear[4] = shear.x;
    m_shear[5] = 1.f+shear.z; // TODO is there a problem here?
    m_shear[6] = shear.y;
}


Grass::Grass() : Grass(glm::vec3(0.f,0.f,0.f)) {}

Grass::Grass(glm::vec3 position) {
    m_points.clear();
    this->position = position;
    glm::vec3 p = position;

    float height = 0.015f;
    float half_width = 0.0075f;
    float r_half_width = 0.005303f;

    // blade 1
    m_points.push_back(glm::vec3(half_width+p.x, 0.f+p.y, height+p.z));
    m_points.push_back(glm::vec3(-half_width+p.x, 0.f+p.y, height+p.z));
    m_points.push_back(glm::vec3(half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_uvs.push_back(glm::vec2(1.f,0.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    //bottom face
    m_points.push_back(glm::vec3(-half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(-half_width+p.x, 0.f+p.y, height+p.z));
    m_uvs.push_back(glm::vec2(0.f,1.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));

    // blade 2
    m_points.push_back(glm::vec3(r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_points.push_back(glm::vec3(-r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_points.push_back(glm::vec3(r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_uvs.push_back(glm::vec2(1.f,0.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    //bottom face
    m_points.push_back(glm::vec3(-r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(-r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_uvs.push_back(glm::vec2(0.f,1.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));

    // blade 3
    m_points.push_back(glm::vec3(r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_points.push_back(glm::vec3(-r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_points.push_back(glm::vec3(r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_uvs.push_back(glm::vec2(1.f,0.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    //bottom face
    m_points.push_back(glm::vec3(-r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_points.push_back(glm::vec3(-r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_uvs.push_back(glm::vec2(0.f,1.f));
    m_uvs.push_back(glm::vec2(1.f,1.f));
    m_uvs.push_back(glm::vec2(0.f,0.f));

}




