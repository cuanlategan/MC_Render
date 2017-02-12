#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 mvpID;

attribute vec3 attr_center;
uniform float time, wavelength, amplitude, steepnes, speed, dirX, dirY;

const float PI = 3.14159265359;

vec4 calcGerstnerWave(float frequency, float QA, float amplitude, vec3 position, vec2 direction,
                                 float time, float phase_const);


void main(){

	// Output position of the vertex, in clip space : mvpID * position
	//gl_Position =  mvpID * vec4(vertexPosition_modelspace,1);

	if(vertexUV.y > .9){
        gl_Position = mvpID * vec4(vertexPosition_modelspace,1);
    }else{
        vec2 dir        = vec2(dirX,dirY);
        float freq      = 2*PI/wavelength;
        float phase_con = freq*speed;
        float QA        = steepnes*amplitude;
        vec4 result     = calcGerstnerWave(freq, QA,amplitude,attr_center, dir, time, phase_con);

        dir = vec2(dirX*0.77,dirY*0.77);
        freq = 2*PI/(wavelength);
        phase_con = freq*speed;
        QA = (steepnes*0.77)*(amplitude*0.7);
        result += calcGerstnerWave(freq, QA,amplitude,attr_center, dir, time, phase_con);

        dir = vec2(dirX*1.33, dirY*1.33);
        freq = 2*PI/(wavelength);
        phase_con = freq*speed;
        QA = (steepnes*1.33)*(amplitude*1.33);
        result += calcGerstnerWave(freq, QA,amplitude,attr_center, dir, time, phase_con);

        result /= 3;

        gl_Position     = mvpID * ((result  +vec4(vertexPosition_modelspace,1)) );

     }

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

vec4 calcGerstnerWave(float frequency, float QA, float amplitude, vec3 position, vec2 direction,
                                 float time, float phase_const)
    {


        float wave_phase = frequency * dot(direction.xy,position.xy) + (time*phase_const);

        float c = cos(wave_phase);
        float s = sin(wave_phase);

        return  vec4(QA*direction.x*c, QA*direction.y*c, amplitude*s, 0.0);
    }