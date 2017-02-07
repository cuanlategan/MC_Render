//
// Created by cuan on 5/26/16.
//

#include "grass.h"


using namespace std;
using namespace glm;



void Grass::setShear(const glm::vec3& shear) {
    m_shear[4] = shear.x;
    m_shear[5] = 1.f+shear.z; // TODO is there a problem here?
    m_shear[6] = shear.y;
}


Grass::Grass() : Grass(glm::vec3(0.f,0.f,0.f)) {}

Grass::Grass(vec3 position) {
    m_points.clear();
    this->position = position;
    vec3 p = position;

    /*float height = 1.f;
    float half_width = 1.f;
    float r_half_width = 0.707107f;*/

    float height = 0.15625f;
    float half_width = 0.078125;
    float r_half_width = 0.055243f;

    /*float height = 0.03125f;
    float half_width = 0.015625f;
    float r_half_width = 0.011049f;*/

    //float height = 0.015f;
    //float half_width = 0.0075f;
    //float r_half_width = 0.005303f;

    // blade 1
    m_points.push_back(vec3(half_width+p.x, 0.f+p.y, height+p.z));
    m_points.push_back(vec3(-half_width+p.x, 0.f+p.y, height+p.z));
    m_points.push_back(vec3(half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(half_width+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(-half_width+p.x, 0.f+p.y, height+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));

    // blade 2
    m_points.push_back(vec3(r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_points.push_back(vec3(-r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_points.push_back(vec3(r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_points.push_back(vec3(r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_points.push_back(vec3(-r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));

    // blade 3
    m_points.push_back(vec3(r_half_width+p.x, r_half_width+p.y, height+p.z));
    m_points.push_back(vec3(-r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_points.push_back(vec3(r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-r_half_width+p.x, -r_half_width+p.y, 0.f+p.z));
    m_points.push_back(vec3(r_half_width+p.x, r_half_width+p.y, 0.f+p.z));
    m_points.push_back(vec3(-r_half_width+p.x, -r_half_width+p.y, height+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));


    /*
    // blade 1
    m_points.push_back(vec3(1.f+p.x, 2.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(-1.f+p.x, 2.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(1.f+p.x, 0.f+p.y, 0.f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-1.f+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(1.f+p.x, 0.f+p.y, 0.f+p.z));
    m_points.push_back(vec3(-1.f+p.x, 2.f+p.y, 0.f+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));

    // blade 2
    m_points.push_back(vec3(0.707107f+p.x, 2.f+p.y, -0.707107f+p.z));
    m_points.push_back(vec3(-0.707107f+p.x, 2.f+p.y, 0.707107f+p.z));
    m_points.push_back(vec3(0.707107f+p.x, 0.f+p.y, -0.707107f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-0.707107f+p.x, 0.f+p.y, 0.707107f+p.z));
    m_points.push_back(vec3(0.707107f+p.x, 0.f+p.y, -0.707107f+p.z));
    m_points.push_back(vec3(-0.707107f+p.x, 2.f+p.y, 0.707107f+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));

    // blade 3
    m_points.push_back(vec3(0.707107f+p.x, 2.f+p.y, 0.707107f+p.z));
    m_points.push_back(vec3(-0.707107f+p.x, 2.f+p.y, -0.707107f+p.z));
    m_points.push_back(vec3(0.707107f+p.x, 0.f+p.y, 0.707107f+p.z));
    m_uvs.push_back(vec2(1.f,0.f));
    m_uvs.push_back(vec2(0.f,0.f));
    m_uvs.push_back(vec2(1.f,1.f));
    //bottom face
    m_points.push_back(vec3(-0.707107f+p.x, 0.f+p.y, -0.707107f+p.z));
    m_points.push_back(vec3(0.707107f+p.x, 0.f+p.y, 0.707107f+p.z));
    m_points.push_back(vec3(-0.707107f+p.x, 2.f+p.y, -0.707107f+p.z));
    m_uvs.push_back(vec2(0.f,1.f));
    m_uvs.push_back(vec2(1.f,1.f));
    m_uvs.push_back(vec2(0.f,0.f));
*/
}


/* if(shear_x_forward){
        shear_x +=0.005f;
    } else { shear_x -=0.005f; }

    if(shear_y_forward){
        shear_y +=0.005f;
    } else { shear_y -=0.005f; }

    if(shear_z_forward){
        shear_z +=0.005f;
    } else { shear_z -=0.005f; }



    if(shear_x >= 0.5f){
        shear_x = 0.5f;
        shear_x_forward = false;
    } else if (shear_x <= -0.5f){
        shear_x = -0.5f;
        shear_x_forward = true;
    }

    if(shear_y >= 0.5f){
        shear_y = 0.5f;
        shear_y_forward = false;
    } else if (shear_y <= -0.5f){
        shear_y = -0.5f;
        shear_y_forward = true;
    }

    if(shear_z >= 0.3f){
        shear_z = 0.3f;
        shear_z_forward = false;
    } else if (shear_z <= -0.3f){
        shear_z = -0.3f;
        shear_z_forward = true;
    }*/

