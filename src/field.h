//
// Created by cuan on 5/31/16.
//

#ifndef CGRA_PROJECT_A3_FIELD_H
#define CGRA_PROJECT_A3_FIELD_H
#pragma once

//#include "cgra_math.hpp"
#include "grass.h"
#include "wave_generator.h"
#include <vector>


class Field {
private:
/*
    std::vector<Grass> *grass_clusters;
    std::vector<glm::vec3> *m_centers;

    std::vector<glm::vec3> *m_points;
    std::vector<glm::vec2> *m_uvs;
    std::vector<glm::vec3> *m_normals;

    float grass_width = 2.f;

    GLuint m_nVBOVertices = 0;
    GLuint m_nVBOTexCoords = 0;
    GLuint m_nVBONormals = 0;

    GLuint g_grass_tex = 0;
    GLuint g_center_attrib = 0;
*/
public:

    std::vector<Grass> *grass_clusters;
    std::vector<glm::vec3> *m_centers;

    std::vector<glm::vec3> *m_points;
    std::vector<glm::vec2> *m_uvs;
    std::vector<glm::vec3> *m_normals;

    float grass_width = 2.f;

    GLuint m_nVBOVertices = 0;
    GLuint m_nVBOTexCoords = 0;
    GLuint m_nVBONormals = 0;

    GLuint g_grass_tex = 0;
    GLuint g_center_attrib = 0;


    float wavelength = 0.133f;
    float amplitude = 0.005f;
    float steepnes = 0.8f;
    float speed = 0.05f;
    int hasTex = 1;
    float dirX = 1.3f;
    float dirY = 0.0f;

    const int GRID_DIMENSION = 50;

    Field() {
        grass_clusters = new std::vector<Grass>;

        m_points = new std::vector<glm::vec3>;
        m_uvs = new std::vector<glm::vec2>;
        m_normals = new std::vector<glm::vec3>;

        m_centers = new std::vector<glm::vec3>;
        m_points->reserve(GRID_DIMENSION * GRID_DIMENSION * 3 * 2 * 3);
        m_normals->reserve(GRID_DIMENSION * GRID_DIMENSION * 3 * 2 * 3);
        m_uvs->reserve(GRID_DIMENSION * GRID_DIMENSION * 3 * 2 * 3);
        m_centers->reserve(GRID_DIMENSION * GRID_DIMENSION);

        
        srand(time(NULL));
    }



    
    void generateCluster(int num_clusters);
    

};


#endif //CGRA_PROJECT_A3_FIELD_H
