#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
using namespace glm;


class Camera {
public:
    Camera();
    Camera(vec3 pos, vec3 lookat, vec3 up);
    vec3 getPos();
    vec3 getLookAt();
    vec3 getUp();
    void rotateCW();
    void rotateCCW();
    void approach();
    void backup();
    void updatePos();
private:
    vec3 _pos;
    vec3 _lookat;
    vec3 _up;
    float _phi;
    float _rad;
    
};