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
#include "ModelController.h"
#include "Axis.h"
#include "ViewController.h"
//Library to load popular file formats and easy integration to project
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Structs.h"
#include "Sphere.h"

using namespace std;
using namespace glm;

const GLuint WIDTH = 1024, HEIGHT = 768;
glm::mat4 projection_matrix;

int depthShaderProgram;

float gridUnit = 1.0f;
int SELECTEDMODELINDEX = 1;
ViewController* viewController = NULL;
ModelController* modelController = NULL;

GLuint toggle = 0; //0 = off, 1 = on
GLuint textureArray[4] = {}; //Contains toggle (on/off), box texture, metal texture, and tiled texture
int shaderType; //Color or texture

//Forward declarations
GLuint loadTexture(const char* filename);
const char* getTexturedVertexShaderSource();
const char* getTexturedFragmentShaderSource();

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
        "uniform mat4 lightSpaceMatrix;"
        ""
        "out vec3 vertexColor;"
        "out vec3 norm;"
        "out vec3 fragPos;"
        "out vec4 fragPosLightSpace;"

        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   fragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        //"   fragPos = aPos;"
        "   norm = mat3(transpose(inverse(worldMatrix))) * aNorm;"
        //"   norm = aNorm;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);"
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
           "in vec4 fragPosLightSpace;"
           "uniform vec3 lightPos;"
           "uniform vec3 viewPos;"
           "uniform sampler2D shadowMap;"
            
        "float ShadowCalculation(vec4 fragPosLSpace)"
        "{"
            // perform perspective divide
            "vec3 projCoords = fragPosLSpace.xyz / fragPosLSpace.w;"
            // transform to [0,1] range
            "projCoords = projCoords * 0.5 + 0.5;"
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            "float closestDepth = texture(shadowMap, projCoords.xy).r;"
            // get depth of current fragment from light's perspective
            "float currentDepth = projCoords.z;"
            // calculate bias (based on depth map resolution and slope)
            "vec3 normal = normalize(norm);"
            "vec3 lightDir = normalize(lightPos - fragPos);"
            "float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);"
            // check whether current frag pos is in shadow
            // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
            // PCF
            "float shadow = 0.0;"
            "vec2 texelSize = 1.0 / textureSize(shadowMap, 0);"
            "for (int x = -1; x <= 1; ++x)"
            "{"
                "for (int y = -1; y <= 1; ++y)"
                "{"
                    "float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;"
                    "shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;"
                "}"
            "}  "
            "shadow /= 9.0;  "

            // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
            "if (projCoords.z > 1.0)"
                "shadow = 0.0;"

            "return shadow;"
         "}"

           "void main()"
           "{"
           "   vec3 color = vertexColor;"
           "   vec3 lightColor = vec3(0.3);" //Bright White
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
           "   vec3 specular = spec * lightColor; "
           // calculate shadow
           "   float shadow = ShadowCalculation(fragPosLightSpace);"
           "   vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; "
           "   FragColor = vec4(lighting, 1.0);"
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
        "uniform mat4 lightSpaceMatrix;"
        ""
        "out vec3 vertexColor;"
        "out vec3 norm;"
        "out vec3 fragPos;"
        "out vec4 fragPosLightSpace;"
        "out vec2 vertexUV;"
        ""
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   fragPos = vec3(worldMatrix * vec4(aPos, 1.0));"
        "   norm = mat3(transpose(inverse(worldMatrix))) * aNorm;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);"
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
        "in vec4 fragPosLightSpace;"
        "uniform vec3 lightPos;"
        "uniform vec3 viewPos;"
        "uniform sampler2D textureSampler;"
        "uniform sampler2D shadowMap;"

        "float ShadowCalculation(vec4 fragPosLSpace)"
        "{"
            // perform perspective divide
            "vec3 projCoords = fragPosLSpace.xyz / fragPosLSpace.w;"
            // transform to [0,1] range
            "projCoords = projCoords * 0.5 + 0.5;"
            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            "float closestDepth = texture(shadowMap, projCoords.xy).r;"
            // get depth of current fragment from light's perspective
            "float currentDepth = projCoords.z;"
            // calculate bias (based on depth map resolution and slope)
            "vec3 normal = normalize(norm);"
            "vec3 lightDir = normalize(lightPos - fragPos);"
            "float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);"
            // check whether current frag pos is in shadow
            // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
            // PCF
            "float shadow = 0.0;"
            "vec2 texelSize = 1.0 / textureSize(shadowMap, 0);"
            "for (int x = -1; x <= 1; ++x)"
            "{"
            "for (int y = -1; y <= 1; ++y)"
            "{"
            "float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;"
            "shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;"
            "}"
            "}  "
            "shadow /= 9.0;  "

            // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
            "if (projCoords.z > 1.0)"
            "shadow = 0.0;"

            "return shadow;"
        "}"

        
        "void main()"
        "{"
        "   vec3 color = vertexColor;"
        "   vec3 lightColor = vec3(0.3);" //Bright White
        //ambiant
        //"   vec3 lightPos = vec3(0.0f, 30.0f, 0.0f);" 
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
        "   vec3 specular = spec * lightColor; "

        // calculate shadow
        "   float shadow = ShadowCalculation(fragPosLightSpace);"
        "   vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; "
        "   vec4 textureColor = texture( textureSampler, vertexUV );"
        "   FragColor = textureColor * vec4(lighting, 1.0);"
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

const char* getDepthVertexShaderSource()
{
    //Vertex Shader
    return
        "#version 330 core\n"
        "layout (location=0) in vec3 aPos;"

        "uniform mat4 lightSpaceMatrix;"
        "uniform mat4 worldMatrix;"
        " void main()"
        " {"
        "   gl_Position = lightSpaceMatrix * worldMatrix * vec4(aPos, 1.0);"
        " }";
}

const char* getDepthFragmentShaderSource()
{
    //Fragment Shaders here
    return
        "#version 330 core\n"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "}";
}

int compileAndLinkDepthShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getDepthVertexShaderSource();
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
    const char* fragmentShaderSource = getDepthFragmentShaderSource();
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

    TexturedColoredVertex(vec3(0.0f, 0.0f, -1.0f), vec3(122.0f/255.0f, 122.0f / 255.0f, 122.0f / 255.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), 
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
void drawGridSquare(int shader, GLuint worldMatrixLocation, float xDisplacement, float yDisplacement, float zDisplacement, float gridUnit, mat4 worldRotationUpdate, GLuint textureArray[]) {

    glm::mat4 translationMatrix = worldRotationUpdate * glm::translate(glm::mat4(1.0f), glm::vec3(xDisplacement, yDisplacement, zDisplacement)); 
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
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
                drawGridSquare(shader, worldMatrixLocation, col * pointDisplacementUnit, 0.0f, row * pointDisplacementUnit, pointDisplacementUnit, worldRotationUpdate, textureArray);
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
                //glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
                glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }
}


/// <summary>
/// Draws Axis lines centered at the origin
/// </summary>
/// <param name="shader"></param>
/// <param name="vao"></param>
/// <param name="gridUnit"></param>
void drawAxisLines(int shader, GLuint vao[], float gridUnit, mat4 worldRotationUpdate) {
    glBindVertexArray(vao[1]);
    mat4 axisMatrix = worldRotationUpdate * mat4(1.0f);

    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    //X-axis
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Y-axis
    glBindVertexArray(vao[2]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Z-axis
    glBindVertexArray(vao[3]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);
}

//Update through user input
//void updateInput(GLFWwindow* window, float dt, vec3& worldRotation, GLuint& toggle)
void updateInput(GLFWwindow* window, float dt, vec3& worldRotation, int shaderArray[])
{
    //Scale
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        modelController->updateScaling(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        modelController->updateScaling(-0.1f);
    }

    //Position
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateX(-0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateX(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateY(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateY(-0.1f);
    }

    //rotation
    if (glfwGetKey(window, GLFW_KEY_A) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) == GLFW_PRESS)
    {
        modelController->updateRotationY(-5.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) == GLFW_PRESS)
    {
        modelController->updateRotationY(5.0f);
    }

    //Rendering mode
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        modelController->updateRenderMode(GL_POINTS);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        modelController->updateRenderMode(GL_LINES);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        modelController->updateRenderMode(GL_TRIANGLES);
    }

    //Focused model selection
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        modelController->modelFocusSwitch(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        modelController->modelFocusSwitch(2);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        modelController->modelFocusSwitch(3);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        modelController->modelFocusSwitch(4);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        modelController->modelFocusSwitch(5);
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

    //small move forward + shear
    if ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
    {
        modelController->updateShearingY(-0.05f);
        modelController->updateZ(-0.1f);
    }
    //small reverse + shear
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateShearingY(0.05f);
        modelController->updateZ(0.1f);
    }

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

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

    const int geometryCount = 8; //number of models to load
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

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, color)); //color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, normal)); //normal
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, uv)); //aUV in vertex shader
    glEnableVertexAttribArray(3);

    //Ground textured Grid
    glBindVertexArray(vaoArray[5]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedGroundVertexArray), texturedGroundVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0); //position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, color)); //color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, normal)); //normal
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, uv)); //aUV in vertex shader
    glEnableVertexAttribArray(3);

    //Sphere
    Sphere sphere(5.5f, 100, 50, shaderProgram);
    sphere.buildSphere();

    glBindVertexArray(vaoArray[6]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[6]);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVerticesSize(), sphere.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)0); //position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, color)); //color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, normal)); //normal
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (void*)offsetof(TexturedColoredVertex, uv)); //aUV in vertex shader
    glEnableVertexAttribArray(3);

    sphere.setVao(vaoArray[6]);
    sphere.setVbo(vboArray[6]);

    // Variables to be used later in tutorial
    float angle = 0;
    float rotationSpeed = 180.0f;  // 180 degrees per second
    float lastFrameTime = glfwGetTime();

    //World Orientation
    vec3 worldRotation(1.f);

    //Cursor position
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);


    //Enable hidden surface removal
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 worldRotationX;
    mat4 worldRotationY;
    mat4 worldRotationUpdate;

    depthShaderProgram = compileAndLinkDepthShaders();

    ViewController view(window, WIDTH, HEIGHT, shaderProgram, shaderArray);
    viewController = &view;

    ModelController model;
    modelController = &model;
    modelController->initModels(shaderProgram, vaoArray[4], vboArray[4], sphere);


    //Initiale Light
    vec3 lightPos = vec3(10.0f, 30.0f, 0.0f);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 75.0f;
    lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); //Light with an angle
    //lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane); //Light fromt the top
    lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    glProgramUniform3fv(shaderProgram, glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
    glProgramUniformMatrix4fv(shaderProgram, glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    //glProgramUniform3fv(texturedShaderProgram, glGetUniformLocation(texturedShaderProgram, "lightPos"), 1, &lightPos[0]);
    //glProgramUniformMatrix4fv(texturedShaderProgram, glGetUniformLocation(texturedShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    glProgramUniformMatrix4fv(depthShaderProgram, glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    glProgramUniform1i(depthShaderProgram, glGetUniformLocation(depthShaderProgram, "shadowMap"), 0/*depthMap*/);

   /* ViewController view(window, WIDTH, HEIGHT, shaderProgram);
    viewController = &view;*/ 
    

    glfwSetWindowSizeCallback(window, framebuffer_size_callback); //Handle window resizing
    glfwSetCursorEnterCallback(window, cursor_enter_callback); //Handle cursor leaving window event: Stop tracking mouse mouvement
    glfwSetCursorPosCallback(window, cursor_position_callback); //Handle cursor mouvement event: Update ViewController's mouse position
    
    viewController->initCamera();

    // Toggle Shadow on/off
    bool toggleShadow = false;

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
        // 1. render depth of scene to texture (from light's perspective)
        glUseProgram(depthShaderProgram);
        
       
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
            
        //Draw all objects
        modelController->drawModels(worldRotationUpdate, textureArray, depthShaderProgram);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderType);
              
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        drawGroundGrid(shaderType, vaoArray, gridUnit, worldRotationUpdate, textureArray);
        axis.drawAxisLines(shaderType, vaoArray, gridUnit, worldRotationUpdate);

        //MODELS
        modelController->drawModels(worldRotationUpdate, textureArray, shaderType);

        //Toggle Shadow
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            if (toggleShadow == true)
            {
                depthShaderProgram = compileAndLinkDepthShaders();
                glProgramUniformMatrix4fv(depthShaderProgram, glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
                toggleShadow = false;
            }
            else
            {
                depthShaderProgram = compileAndLinkDepthShaders();
                glProgramUniformMatrix4fv(depthShaderProgram, glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 0, GL_FALSE, &lightSpaceMatrix[0][0]);
                toggleShadow = true;
            }
        }

        viewController->setFastCam(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS); //Press shift to go faster
        viewController->update(shaderType);
                                                                                                                                           
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    viewController->~ViewController();
    modelController->~ModelController();
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
