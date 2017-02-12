#ifndef CAMERA_H
#define CAMERA_H

class Camera {
    public:
        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        //glm::mat4 getProjectionMatrix();
};


#endif