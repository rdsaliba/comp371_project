#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
using namespace glm;


class Axis {
public:
    Axis();
    Axis(float size, float gridUnit, GLuint * vaoArray, GLuint * vboArray);
    void drawAxisLines(int shader, GLuint vao[], float gridUnit, mat4 worldRotationUpdate);
    void bindAxis();
private:
    vec3 _xAxisVertexArray[4];
    vec3 _yAxisVertexArray[4];
    vec3 _zAxisVertexArray[4];
    GLuint* _vaoArray;
    GLuint* _vboArray;

};

