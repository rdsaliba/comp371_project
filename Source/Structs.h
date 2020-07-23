#pragma once
#include <glm/glm.hpp>
#include <string>
using namespace std;
/// <summary>
/// Taken as part of Mesh implementation from learn openGL: https://learnopengl.com/Model-Loading/Mesh
/// </summary>
struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};