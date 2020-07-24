#include <iostream>
#include <list>
#include <algorithm>
#define GLFW_PRESS 1
#define GLFW_MOUSE_PRESS_BUTTON_RIGHT GLFW_MOUSE_BUTTON_2
#define GLFW_MOUSE_PRESS_BUTTON_LEFT GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_2
#pragma comment (lib,"glew32s.lib")
#define GLEW_STATIC // This allows linking with Static Library on Windows, without DLL

#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include "Model.h"
#include "Axis.h"
#include "HauModel.h"
#include "RoyModel.h"
#include "TaqiModel.h"
#include "SwetangModel.h"
#include "WilliamModel.h"
#include "ViewController.h"

//Library to load popular file formats and easy integration to project
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

const GLuint WIDTH = 1024, HEIGHT = 768;
glm::mat4 projection_matrix;

float gridUnit = 1.0f;
void modelFocusSwitch(int nextModel);
int SELECTEDMODELINDEX = 1;
Model* focusedModel = NULL;
ViewController* viewController = NULL;
Model models[] = {
    Model(vec3(0.0f, 0.0f, 0.0f), 0.0f), //axis lines
    TaqiModel(vec3(-45.0f, 0.0f, -45.0f), 0.0f), //Taqi (Q4)
    HauModel(vec3(45.0f, 0.0f, -45.0f), 0.0f), //Hau (U6)
    RoyModel(vec3(-45.0f, 0.0f, 45.0f), 0.0f), //Roy (Y8)
    SwetangModel(vec3(0.0f, 0.0f, 0.0f), 0.0f), //Swetang (E0)
    WilliamModel(vec3(45.0f, 0.0f, 45.0f), 0.0f) //William (L9)
};

GLuint toggle = 0; //0 = off, 1 = on
GLuint textureArray[4] = {}; //Contains toggle (on/off), box texture, metal texture, and tiled texture
int shaderType; //Color or texture

//Forward declarations
GLuint loadTexture(const char* filename);
const char* getTexturedVertexShaderSource();
const char* getTexturedFragmentShaderSource();

//UV coordinates with vertex data
struct TexturedColoredVertex
{
    //Position of vertex, color of vertex, UV coordinate for that vertex
    TexturedColoredVertex(vec3 _position, vec3 _color, vec3 _normal, vec2 _uv) : position(_position), color(_color), normal(_normal), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec3 normal;
    vec2 uv;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    //update projection matrix to new width and height
    projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
}

void cursor_enter_callback(GLFWwindow* window, int entered) {
    if (entered) {
        double mousePosX, mousePosY;
       glfwGetCursorPos(window, &mousePosX, &mousePosY);
       viewController->setMousePosX(mousePosX);
       viewController->setMousePosY(mousePosY);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    viewController->setMousePosX(xpos);
    viewController->setMousePosY(ypos);
}

const char* getVertexShaderSource()
{
    //Vertex Shader
    return
        "#version 330 core\n"
        "layout (location=0) in vec3 aPos;"
        "layout (location=1) in vec3 aColor;"
        "layout (location=2) in vec3 aNorm;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);" //default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "out vec3 norm;"
        "out vec3 fragPos;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   fragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        //"   fragPos = aPos;"
        "   norm = mat3(transpose(inverse(worldMatrix))) * aNorm;"
        //"   norm = aNorm;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}

const char* getFragmentShaderSource()
{
    //Fragment Shaders here
    return
        "#version 330 core\n"
           "out vec4 FragColor;"

           "in vec3 fragPos;"
           "in vec3 vertexColor;"
           "in vec3 norm;"
           

           "uniform vec3 viewPos;"


           "void main()"
           "{"
           "   vec3 color = vertexColor;"
           "   vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);"
           // ambient
           "   vec3 ambient = 0.05 * color;"  //0.05
           // diffuse
           "   vec3 lightDir = normalize(lightPos - fragPos);"
           "   vec3 normal = normalize(norm);"
           "   float diff = max(dot(lightDir, normal), 0.0);"
           "   vec3 diffuse = diff * color;"
           // specular
           "   vec3 viewDir = normalize(viewPos - fragPos);"
           "   float spec = 0.0;"
           "   vec3 reflectDir = reflect(-lightDir, normal);"
           "   spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);"
           "   vec3 specular = vec3(0.3) * spec;" // assuming bright white light color
           "   FragColor = vec4(ambient + diffuse + specular, 1.0);"
           " }";
}

//Vertex shader receives UV attributes and outputs them back to fragment shader
const char* getTexturedVertexShaderSource()
{
    // For now, you use a string for your shader code, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        "layout (location = 2) in vec3 aNorm;"
        "layout (location = 3) in vec2 aUV;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "out vec3 norm;"
        "out vec3 fragPos;"
        "out vec2 vertexUV;"
        ""
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   fragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        "   norm = mat3(transpose(inverse(worldMatrix))) * aNorm;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "   vertexUV = aUV;"
        "}";
}

//Fragment shader fetches the texture sample at coordinate UV and blends the color with vertex colors
const char* getTexturedFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "out vec4 FragColor;"
        "in vec3 fragPos;"
        "in vec3 vertexColor;"
        "in vec2 vertexUV;"
        "in vec3 norm;"
        "uniform vec3 viewPos;"
        "uniform sampler2D textureSampler;"
        ""
        "void main()"
        "{"
        "   vec3 color = vertexColor;"
        "   vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);"
        //ambiant
        "   vec3 ambient = 0.05 * color;"  //0.05
        //diffuse
        "   vec3 lightDir = normalize(lightPos - fragPos);"
        "   vec3 normal = normalize(norm);"
        "   float diff = max(dot(lightDir, normal), 0.0);"
        "   vec3 diffuse = diff * color;"
        //specular
        "   vec3 viewDir = normalize(viewPos - fragPos);"
        "   float spec = 0.0;"
        "   vec3 reflectDir = reflect(-lightDir, normal);"
        "   spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);"
        "   vec3 specular = vec3(0.3) * spec;" // assuming bright white light color

        "   vec4 textureColor = texture( textureSampler, vertexUV );"
        "   FragColor = textureColor * vec4(ambient + diffuse + specular, 1.0f);"
        "}";
}


int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

// Textured Cube model
const TexturedColoredVertex texturedCubeVertexArray[] = {  // position, color, normal, UV coordinates
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
};

//Square plane
const TexturedColoredVertex texturedGroundVertexArray[] = {  // position, color, normal, UV coordinates

    TexturedColoredVertex(vec3(0.0f, 0.0f, -1.0f), vec3(122.0f/255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3(-1.0f, 0.0f,-1.0f), vec3(122.0f / 255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f), vec3(122.0f / 255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.0f, 0.0f, 0.0f), vec3(122.0f / 255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-1.0f, 0.0f,-1.0f), vec3(122.0f / 255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-1.0f, 0.0f, 0.0f), vec3(122.0f / 255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
};

vec3 gridVertexArray[] = {
    //ground unit square vertices
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(gridUnit, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(gridUnit, 0.0f, gridUnit),
    glm::vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, gridUnit),
    glm::vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(5 * gridUnit, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),


    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 5 * gridUnit,0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 5 * gridUnit),
    glm::vec3(0.0f, 0.0f, 1.0f),vec3(0.0f, 1.0f, 0.0f)
};

/// <summary>
/// draws 1 ground square
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="xDisplacement"></param>
/// <param name="yDisplacement"></param>
/// <param name="zDisplacement"></param>
void drawGridSquare(GLuint worldMatrixLocation, float xDisplacement, float yDisplacement, float zDisplacement, float gridUnit, mat4 worldRotationUpdate, GLuint textureArray[]) {

    glm::mat4 translationMatrix = worldRotationUpdate * glm::translate(glm::mat4(1.0f), glm::vec3(xDisplacement, yDisplacement, zDisplacement)); 
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}


/// <summary>
/// Generates ground grid
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="pointDisplacementUnit">Length of vertex</param>
void drawGroundGrid(int shader, GLuint vao[], float pointDisplacementUnit, mat4 worldRotationUpdate, GLuint textureArray[]) {    

    mat4 worldMatrix = worldRotationUpdate * mat4(1.0f);
    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");

    //Ground without texture
    if (textureArray[0] == 0)
    {
        glBindVertexArray(vao[0]);
        for (int row = -50; row < 50; row++) {
            for (int col = -50; col < 50; col++) {
                drawGridSquare(worldMatrixLocation, col * pointDisplacementUnit, 0.0f, row * pointDisplacementUnit, pointDisplacementUnit, worldRotationUpdate, textureArray);
            }
        }
    }

    //Ground with texture
    else if (textureArray[0] == 1)
    {
        glBindTexture(GL_TEXTURE_2D, textureArray[3]);

        glBindVertexArray(vao[5]);

        for (int row = -49; row < 51; row++) 
        {
            for (int col = -49; col < 51; col++) 
            {
                glm::mat4 translationMatrix = worldRotationUpdate * glm::translate(glm::mat4(1.0f), glm::vec3(row, 0, col));
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }
}

/// <summary>
/// Draws Hau's model (U6)
/// </summary>
/// <param name="shader"></param>
/// <param name="vao"></param>
void drawHauModel(int shader, GLuint vao[], mat4 worldRotationUpdate, GLuint textureArray[]) {
    //Model model(models[2]);
    //HauModel hau(model);
    //hau.draw(worldRotationUpdate);
    Model model = models[2];
    HauModel hau(model.getPosition(), model.getScaling());
    hau.setShaderProgram(shader);
    hau.setVao(vao[4]);
    hau.setRotation(model.getRotation());
    hau.setRenderMode(model.getRenderMode());
    hau.draw(worldRotationUpdate, textureArray);
}

//WILLIAM'S MODEL ("L9")
void drawWilliamModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate, GLuint textureArray[])
{
    Model model = models[5];
    WilliamModel william(model.getPosition(), model.getScaling());
    william.setShaderProgram(shaderProgram);
    william.setVao(vao[4]);
    william.setRotation(model.getRotation());
    william.setRenderMode(model.getRenderMode());
    william.draw(worldRotationUpdate, textureArray);
}

//TAQI'S MODEL ("Q4")
void drawTaqiModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate, GLuint textureArray[])
{
    Model model = models[1];
    TaqiModel taqi(model.getPosition(), model.getScaling());
    taqi.setShaderProgram(shaderProgram);
    taqi.setVao(vao[4]);
    taqi.setRotation(model.getRotation());
    taqi.setRenderMode(model.getRenderMode());
    taqi.draw(worldRotationUpdate, textureArray);
}

//ROY'S MODEL ("Y8")
void drawRoyModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate, GLuint textureArray[])
{
    Model model = models[3];
    RoyModel roy(model.getPosition(), model.getScaling());
    roy.setShaderProgram(shaderProgram);
    roy.setVao(vao[4]);
    roy.setRotation(model.getRotation());
    roy.setRenderMode(model.getRenderMode());
    roy.draw(worldRotationUpdate, textureArray);
}

//Swetang Model "E0"
void drawSwetangModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate, GLuint textureArray[])
{
    Model model = models[4];
    SwetangModel swetang(model.getPosition(), model.getScaling());
    swetang.setShaderProgram(shaderProgram);
    swetang.setVao(vao[4]);
    swetang.setRotation(model.getRotation());
    swetang.setRenderMode(model.getRenderMode());
    swetang.draw(worldRotationUpdate, textureArray);
}


//Update through user input
//void updateInput(GLFWwindow* window, float dt, vec3& worldRotation, GLuint& toggle)
void updateInput(GLFWwindow* window, float dt, vec3& worldRotation, int shaderArray[])
{
    //Scale
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        (*focusedModel).updateScaling(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        (*focusedModel).updateScaling(-0.1f);
    }

    //Position
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        (*focusedModel).x(-0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        (*focusedModel).x(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        (*focusedModel).y(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        (*focusedModel).y(-0.1f);
    }

    //rotation
    if (glfwGetKey(window, GLFW_KEY_A) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) == GLFW_PRESS)
    {
        focusedModel->updateRotationY(-5.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) == GLFW_PRESS)
    {
        focusedModel->updateRotationY(5.0f);
    }

    //Rendering mode
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        focusedModel->setRenderMode(GL_POINTS);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        focusedModel->setRenderMode(GL_LINES);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        focusedModel->setRenderMode(GL_TRIANGLES);
    }

    //Focused model selection
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        modelFocusSwitch(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        modelFocusSwitch(2);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        modelFocusSwitch(3);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        modelFocusSwitch(4);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        modelFocusSwitch(5);
    }


    //World orientation
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        worldRotation.x += 5.0f * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        worldRotation.x -= 5.0f * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        worldRotation.y += 5.0f * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        worldRotation.y -= 5.0f * dt;
    }
    //Home button resets world orientation
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        worldRotation.x = 0;
        worldRotation.y = 0;
    }

    //Texture toggle
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {    
        if (toggle == 0)
        {
            toggle = 1; //Texture is on
            textureArray[0] = toggle; 

            shaderType = shaderArray[1]; //Shader for texture
            glUseProgram(shaderType);
            glActiveTexture(GL_TEXTURE0);
            GLuint textureLocation = glGetUniformLocation(shaderType, "textureSampler");
            glUniform1i(textureLocation, 0);  // Set our Texture sampler to user Texture Unit 0
        }
        else if (toggle == 1)
        {
            toggle = 0; //Texture is off
            textureArray[0] = toggle;
            shaderType = shaderArray[0]; //Shader for color
        }
    }
}

/// <summary>
/// Swaps model being controlled by user
/// </summary>
/// <param name="nextModel">Index of selected model to focus movement on</param>
void modelFocusSwitch(int nextModel)
{
    //Don't update anything if model to switch to is current model
    if (SELECTEDMODELINDEX == nextModel) {
        return;
    }
    //Update pointer to the selected model
    focusedModel = &models[nextModel];
    SELECTEDMODELINDEX = nextModel;

}


int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 1024x768
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Comp371 - Project", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Load Textures
    #if defined(PLATFORM_OSX)
        GLuint boxTextureID = loadTexture("Textures/box_texture.png");
        GLuint metalTextureID = loadTexture("Textures/metal_finish.jpg"); 
        GLuint tiledTextureID = loadTexture("Textures/tiled_texture.png"); 
    #else
        GLuint boxTextureID = loadTexture("../Assets/Textures/box_texture.png"); //Source: https://jooinn.com/wood-texture-box.html
        GLuint metalTextureID = loadTexture("../Assets/Textures/metal_finish.jpg"); //Source: https://unsplash.com/photos/v6uiP2MD6vs
        GLuint tiledTextureID = loadTexture("../Assets/Textures/tiled_texture.png"); //Source: https://www.3dxo.com/textures/tiles
    #endif

    //Array of textures
    textureArray[0] = toggle; // 0 (texture off) or 1 (texture on)
    textureArray[1] = boxTextureID;
    textureArray[2] = metalTextureID;
    textureArray[3] = tiledTextureID;

    // Black background
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    int texturedShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());
    int shaderArray[2] = { shaderProgram, texturedShaderProgram };
    shaderType = shaderArray[0]; //Initial shader is the one with color, without texture

    glm::mat4 projectionMatrix = glm::perspective(70.0f, 1024.0f / 768.0f, 0.01f, 100.0f);

    setProjectionMatrix(shaderArray[0], projectionMatrix);
    setProjectionMatrix(shaderArray[1], projectionMatrix);

    const int geometryCount = 6; //number of models to load
    GLuint vaoArray[geometryCount], vboArray[geometryCount];
    glGenVertexArrays(geometryCount, &vaoArray[0]);
    glGenBuffers(geometryCount, &vboArray[0]);


    //Ground Grid
    glBindVertexArray(vaoArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertexArray), gridVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(2);

    //Axis
    Axis axis = Axis(0.0f, gridUnit, vaoArray, vboArray);
    axis.bindAxis(); //Vao 1, 2, 3

    //Cube (for individual models)
    glBindVertexArray(vaoArray[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0); //position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)sizeof(glm::vec3)); //color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2 * sizeof(glm::vec3))); //normal
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(3 * sizeof(vec3))); //aUV in vertex shader
    glEnableVertexAttribArray(3);

    models[1].setVbo(vboArray[4]);
    models[2].setVbo(vboArray[4]);
    models[3].setVbo(vboArray[4]);
    models[4].setVbo(vboArray[4]);
    models[5].setVbo(vboArray[4]);

    //Ground textured Grid
    glBindVertexArray(vaoArray[5]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedGroundVertexArray), texturedGroundVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(2 * sizeof(vec3))); 
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)(3 * sizeof(vec3))); 
    glEnableVertexAttribArray(3);

    // Variables to be used later in tutorial
    float angle = 0;
    float rotationSpeed = 180.0f;  // 180 degrees per second
    float lastFrameTime = glfwGetTime();

    //World Orientation
    vec3 worldRotation(1.f);

    //Cursor position
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


    focusedModel = &models[1];
    //Enable hidden surface removal
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 worldRotationX;
    mat4 worldRotationY;
    mat4 worldRotationUpdate;

    ViewController view(window, WIDTH, HEIGHT, shaderProgram, shaderArray);
    viewController = &view;

    glfwSetWindowSizeCallback(window, framebuffer_size_callback); //Handle window resizing
    glfwSetCursorEnterCallback(window, cursor_enter_callback); //Handle cursor leaving window event: Stop tracking mouse mouvement
    glfwSetCursorPosCallback(window, cursor_position_callback); //Handle cursor mouvement event: Update ViewController's mouse position
    viewController->initCamera();

     // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        worldRotationX = rotate(glm::mat4(1.0f), glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        worldRotationY = rotate(glm::mat4(1.0f), glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        worldRotationUpdate = worldRotationX * worldRotationY;

        //Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        viewController->updateDt(dt);

        //Get user inputs
        updateInput(window, dt, worldRotation, shaderArray);

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        axis.drawAxisLines(shaderType, vaoArray, gridUnit, worldRotationUpdate);
        drawGroundGrid(shaderType, vaoArray, gridUnit, worldRotationUpdate, textureArray);

        //MODELS
        drawTaqiModel(shaderType, vaoArray, worldRotationUpdate, textureArray);
        drawHauModel(shaderType, vaoArray, worldRotationUpdate, textureArray);
        drawRoyModel(shaderType, vaoArray, worldRotationUpdate, textureArray);
        drawSwetangModel(shaderType, vaoArray, worldRotationUpdate, textureArray);
        drawWilliamModel(shaderType, vaoArray, worldRotationUpdate, textureArray);

        viewController->setFastCam(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS); //Press shift to go faster
        viewController->update(shaderType);
                                                                                                                                           
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

//Load the texture
GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}
