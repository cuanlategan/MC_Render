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



void Field::addPatch(float x, float y, float z){

    const float height = 0.8f;
    const float half_width = 0.3f;

    vector<vec3> blade_vert(6);
    blade_vert.push_back(vec3( -half_width, 0.f,    0.f ));
    blade_vert.push_back(vec3( half_width,  0.f,    0.f ));
    blade_vert.push_back(vec3( half_width,  height, 0.f ));
    blade_vert.push_back(vec3( -half_width, 0.f,    0.f ));
    blade_vert.push_back(vec3( half_width,  height, 0.f ));
    blade_vert.push_back(vec3( -half_width, height, 0.f ));

    vector<vec2> blade_uv(6);
    blade_uv.push_back(vec2( 1.f,1.f ));
    blade_uv.push_back(vec2( 0.f,1.f ));
    blade_uv.push_back(vec2( 0.f,0.f ));
    blade_uv.push_back(vec2( 1.f,1.f ));
    blade_uv.push_back(vec2( 0.f,0.f ));
    blade_uv.push_back(vec2( 1.f,0.f ));

    positions->push_back(vec3(x,y,z));
    
    for(auto v: blade_vert){
        m_points->push_back(vec3(v.x+x, v.y+y, v.z+z));
    }
    for(auto v: blade_vert){
        vec3 tmp = rotateY(v,radians(45.f));
        m_points->push_back(vec3(tmp.x+x,tmp.y+y,tmp.z+z));
    }
    for(auto v: blade_vert){
        vec3 tmp = rotateY(v,radians(-45.f));
        m_points->push_back(vec3(tmp.x+x,tmp.y+y,tmp.z+z));
    }

    for(auto v: blade_uv){
        m_uvs->push_back(v);
    }
    for(auto v: blade_uv){
        m_uvs->push_back(v);
    }
    for(auto v: blade_uv){
        m_uvs->push_back(v);
    }

    
}




