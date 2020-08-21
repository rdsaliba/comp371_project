#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
using namespace glm;


class Light {
public:
    Light();
    Light(vec3 pos, vec3 lookAt, vec3 up, float near, float far);
    vec3 getPos();
    vec3 getLookAt();
    vec3 getUp();
    mat4 getProj();
    mat4 getSpaceMatrix();
    void rotateCW();
    void rotateCCW();
    void approach();
    void backup();
    void updatePos();
    void updateLVM();
    bool _on;

private:

    vec3 _pos;
    vec3 _lookAt;
    vec3 _up;
    float _near;
    float _far;
    mat4 _proj;
    mat4 _view;
    mat4 _spaceMatrix;
    float _rad;
    float _phi;

};