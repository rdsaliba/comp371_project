#pragma once
#include <glm/glm.hpp>
#include <string>
using namespace std;
using namespace glm;
/// <summary>
/// Taken as part of Mesh implementation from learn openGL: https://learnopengl.com/Model-Loading/Mesh
/// </summary>
struct TexturedColoredVertex {
    TexturedColoredVertex():position(vec3(0)), color(vec3(0)), normal(vec3(0)), uv(vec3(0)){}
    //Position of vertex, color of vertex, UV coordinate for that vertex
    TexturedColoredVertex(vec3 _position, vec3 _color, vec3 _normal, vec2 _uv) : position(_position), color(_color), normal(_normal), uv(_uv) {}
    // position
    glm::vec3 position;
    // normal
    glm::vec3 normal;
    // color
    glm::vec3 color;
    // uv
    glm::vec2 uv;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};