//
// Created by cuan on 29/05/16.
//

#ifndef CGRA_PROJECT_A3_WAVE_GENERATOR_H
#define CGRA_PROJECT_A3_WAVE_GENERATOR_H

#include <math.h>
#include <vector>
#include <iostream>

#include "cgra_math.hpp"


struct Gerstner {

    float amplitude;
    glm::vec2 direction;
    //float time;
    float frequency;
    float phase_const;
    float QA;
};

class WaveGenerator {
private:

    std::vector<Gerstner> gerst_waves;



    glm::vec3 calcGerstnerWave(float frequency, float QA, float amplitude, const glm::vec2& position, const glm::vec2& direction,
                                 float time, float phase_const)
    {


        float wave_phase = frequency * glm::dot(direction,position) + (time*phase_const);

        float c = float(cos(wave_phase));
        float s = float(sin(wave_phase));

        return  glm::vec3(QA*direction.x*c, QA*direction.y*c,amplitude*s);
    }


public:

    void addGerstnerWave(float wavelength, float amplitude,float steepnes, float speed, glm::vec2 direction)
    {
        Gerstner g;

        g.amplitude = amplitude;
        g.direction = normalize(direction);

        g.frequency = float(2*M_PI) / wavelength;
        g.phase_const = g.frequency * speed;

        float steepnes_ = steepnes;

        if(steepnes_ > 1.f/(g.frequency*amplitude))
        {
            steepnes_ = 1.f/(g.frequency*amplitude);
        }
        g.QA = steepnes_*amplitude;

        gerst_waves.push_back(g);
    }



    glm::vec3 getGroupTotal(const glm::vec2&  position, float time){

        glm::vec3 result(0.f,0.f,0.f);

        float count(0.f);
        for(auto &w: gerst_waves){
            count += 1.f;
            //result += calcGerstnerWave2(w.wavelength, w.amplitude, position, w.direction,w.angle,w.steepnes,time,w.phase);
            result += calcGerstnerWave(w.frequency,w.QA,w.amplitude,position,w.direction,time,w.phase_const);
        }
        result.x /= count;
        result.y /= count;
        result.z /= count;

        return result;
    }


};


#endif //CGRA_PROJECT_A3_WAVE_GENERATOR_H
