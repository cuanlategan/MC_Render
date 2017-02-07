//
// Created by cuan on 5/31/16.
//

#include "field.h"
#include "simple_image.hpp"
#include <GL/gl3w.h>
#include <GL/glu.h>



void Field::generateCluster(int num_clusters) {

    for (int i = 0; i < num_clusters; i++) {
        for (int j = 0; j < num_clusters; j++) {

            float x = float(i);
            float y = float(j);
            //float x = float(i) - num_clusters / 2;
            //float y = float(j) - num_clusters / 2;
            //float x = float(i) +float((rand()%100)/200.f) -num_clusters/2;
            //float y = float(j) +float((rand()%100)/200.f) -num_clusters/2;
            Grass grass(glm::vec3(x, y, 0.f));
            grass_clusters->push_back(grass);


            for (auto &point :grass.getPoints()) {
                m_points->push_back(point);

                glm::vec3 center(grass.getPosition().x*0.5,
                                  grass.getPosition().y*0.5,
                                  grass.getPosition().z*0.5);
                m_centers->push_back(center);
                m_normals->push_back(glm::vec3(0.f,1.f,0.f));
                //std::cout << center << "\n";
            }

            for (auto &uv :grass.getUvs()) {
                m_uvs->push_back(uv);
            }
            //m_centers->push_back(grass.getPosition());


        }
    }
    int max_attributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes);
    std::cout << "GL_MAX_VERTEX_ATTRIBS " << max_attributes << "\n";
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes);
    std::cout << "GL_MAX_VERTEX_ATTRIBS " << max_attributes << "\n";

    

    /*for(auto &p: *m_points){
        std::cout << p << "\n";
    }*/

}









