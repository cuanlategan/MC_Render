#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera{
    public:
        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        //void move(Dir direction);

        
        
};


#endif