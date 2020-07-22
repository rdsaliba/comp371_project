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


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
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
    const char* fragmentShaderSource = getFragmentShaderSource();
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

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}


vec3 cubeVertexArray[] = {
    // position, color
    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),//left-  red
    vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),

    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),// far - blue
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), // bottom - turquoise
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f),

    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),// near - green
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),// right - purple
    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),

    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),
    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),// top - yellow
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)
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

vec3 xAxisVertexArray[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(5 * gridUnit, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f)
};

vec3 yAxisVertexArray[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 5 * gridUnit,0.0f),
    glm::vec3(0.0f, 1.0f, 1.0f)
};

vec3 zAxisVertexArray[] = {
     glm::vec3(0.0f, 0.0f, 0.0f),
     glm::vec3(0.0f, 0.0f, 1.0f),
     glm::vec3(0.0f, 0.0f, 5 * gridUnit),
     glm::vec3(0.0f, 0.0f, 1.0f)
};

/// <summary>
/// draws 1 ground square
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="xDisplacement"></param>
/// <param name="yDisplacement"></param>
/// <param name="zDisplacement"></param>
void drawGridSquare(GLuint worldMatrixLocation, float xDisplacement, float yDisplacement, float zDisplacement, float gridUnit, mat4 worldRotationUpdate) {

    glm::mat4 translationMatrix = worldRotationUpdate * glm::translate(glm::mat4(1.0f), glm::vec3(xDisplacement, yDisplacement, zDisplacement)); //Note: Multiplying with worldRotationUpdate causes lag
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/// <summary>
/// Generates ground grid
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="pointDisplacementUnit">Length of vertex</param>
void drawGroundGrid(int shader, GLuint vao[], float pointDisplacementUnit, mat4 worldRotationUpdate) {

    glBindVertexArray(vao[0]);
    mat4 worldMatrix = worldRotationUpdate * mat4(1.0f);
    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    for (int row = -50; row < 50; row++) {
        for (int col = -50; col < 50; col++) {
            drawGridSquare(worldMatrixLocation, col * pointDisplacementUnit, 0.0f, row * pointDisplacementUnit, pointDisplacementUnit, worldRotationUpdate);
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
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Y-axis
    glBindVertexArray(vao[2]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Z-axis
    glBindVertexArray(vao[3]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);
}


/// <summary>
/// Draws Hau's model (U6)
/// </summary>
/// <param name="shader"></param>
/// <param name="vao"></param>
void drawHauModel(int shader, GLuint vao[], mat4 worldRotationUpdate) {
    //Model model(models[2]);
    //HauModel hau(model);
    //hau.draw(worldRotationUpdate);
    Model model = models[2];
    HauModel hau(model.getPosition(), model.getScaling());
    hau.setShaderProgram(shader);
    hau.setVao(vao[4]);
    hau.setRotation(model.getRotation());
    hau.setRenderMode(model.getRenderMode());
    hau.draw(worldRotationUpdate);
}

//WILLIAM'S MODEL ("L9")
void drawWilliamModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate)
{
    Model model = models[5];
    WilliamModel william(model.getPosition(), model.getScaling());
    william.setShaderProgram(shaderProgram);
    william.setVao(vao[4]);
    william.setRotation(model.getRotation());
    william.setRenderMode(model.getRenderMode());
    william.draw(worldRotationUpdate);
}

//TAQI'S MODEL ("Q4")
void drawTaqiModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate)
{
    Model model = models[1];
    TaqiModel taqi(model.getPosition(), model.getScaling());
    taqi.setShaderProgram(shaderProgram);
    taqi.setVao(vao[4]);
    taqi.setRotation(model.getRotation());
    taqi.setRenderMode(model.getRenderMode());
    taqi.draw(worldRotationUpdate);
}

//ROY'S MODEL ("Y8")
void drawRoyModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate)
{
    Model model = models[3];
    RoyModel roy(model.getPosition(), model.getScaling());
    roy.setShaderProgram(shaderProgram);
    roy.setVao(vao[4]);
    roy.setRotation(model.getRotation());
    roy.setRenderMode(model.getRenderMode());
    roy.draw(worldRotationUpdate);
}

//Swetang Model "E0"
void drawSwetangModel(int shaderProgram, GLuint vao[], mat4 worldRotationUpdate)
{
    Model model = models[4];
    SwetangModel swetang(model.getPosition(), model.getScaling());
    swetang.setShaderProgram(shaderProgram);
    swetang.setVao(vao[4]);
    swetang.setRotation(model.getRotation());
    swetang.setRenderMode(model.getRenderMode());
    swetang.draw(worldRotationUpdate);
}


//Update through user input
void updateInput(GLFWwindow* window, float dt, vec3& worldRotation)
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
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        (*focusedModel).x(-0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        (*focusedModel).x(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        (*focusedModel).y(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        (*focusedModel).y(-0.1f);
    }

    //rotation
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        focusedModel->updateRotationY(-5.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
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

    // Black background
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();
    glUseProgram(shaderProgram);

    const int geometryCount = 5; //number of models to load
    GLuint vaoArray[geometryCount], vboArray[geometryCount];
    glGenVertexArrays(geometryCount, &vaoArray[0]);
    glGenBuffers(geometryCount, &vboArray[0]);

    // Define and upload geometry for all our models to the GPU
    Axis axis = Axis(0.0f, gridUnit, vaoArray, vboArray);
    axis.bindAxis();

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



    //Axis lines
    //X
    glBindVertexArray(vaoArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertexArray), xAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Y
    glBindVertexArray(vaoArray[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(yAxisVertexArray), yAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Z
    glBindVertexArray(vaoArray[3]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertexArray), zAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Cube (for individual models)
    glBindVertexArray(vaoArray[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexArray), cubeVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    models[1].setVbo(vboArray[4]);
    models[2].setVbo(vboArray[4]);
    models[3].setVbo(vboArray[4]);
    models[4].setVbo(vboArray[4]);
    models[5].setVbo(vboArray[4]);

    // Variables to be used later in tutorial
    float angle = 0;
    float rotationSpeed = 180.0f;  // 180 degrees per second
    float lastFrameTime = glfwGetTime();

    //World Orientation
    vec3 worldRotation(1.f);

    //Cursor position
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    focusedModel = &models[2];
    //Enable hidden surface removal
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 worldRotationX;
    mat4 worldRotationY;
    mat4 worldRotationUpdate;

    ViewController view(window, WIDTH, HEIGHT, shaderProgram);
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
        updateInput(window, dt, worldRotation);

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawGroundGrid(shaderProgram, vaoArray, gridUnit, worldRotationUpdate);
        axis.drawAxisLines(shaderProgram, vaoArray, gridUnit, worldRotationUpdate);

        //MODELS
        drawTaqiModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawHauModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawRoyModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawSwetangModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawWilliamModel(shaderProgram, vaoArray, worldRotationUpdate);

        //FPS camera
        viewController->setFastCam(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS); //Press shift to go faster
        viewController->update();
                                                                                                                                           
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
