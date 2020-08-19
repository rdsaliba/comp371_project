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

#include <stdlib.h>
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include "ModelController.h"
#include "Axis.h"
#include "ViewController.h"
#include "Structs.h"
#include "Sphere.h"
#include "TextureLoader.h"
#include "shaderloader.h"


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
GLuint textureArray[35] = {}; //Contains toggle (on/off), and the cubes textures
GLuint timerTexture[10] = {}; //Texture for timer
int shaderType; //Color or texture

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

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)), //left - white
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.33f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.33f, 0.5f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.33f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.5f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.33f, 0.0f)), // far - white
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.67f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.67f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.33f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.33f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.67f, 0.5f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.5f)), // bottom - white
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.67f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.67f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.67f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.33f, 0.5f)), // near - white
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.33f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.67f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.67f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.33f, 0.5f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.67f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), // right - white
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.33f, 0.5f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.33f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.33f, 0.5f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.5f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - white
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.67f, 0.5f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.67f, 0.5f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.67f, 1.0f))
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

    // move forward + shear
    if ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) && !((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)))
    {
        modelController->updateShearingY(-0.05f);
        modelController->updateZ(-0.1f);
    }
    // reverse + shear
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        modelController->updateShearingY(0.05f);
        modelController->updateZ(0.1f);
    }

    //random location model
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        float setRandNumX = 0.0f;
        float setRandNumZ = 0.0f;
        setRandNumX = rand() % 100 - 50;
        setRandNumZ = rand() % 100 - 50;

        modelController->randomPosition(vec3(setRandNumX, 0, setRandNumZ));
    }

    if (glfwGetKey(window, GLFW_KEY_I)){
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::L_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::L);
    }

    if (glfwGetKey(window, GLFW_KEY_O)){
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::R_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::R);
    }

    if (glfwGetKey(window, GLFW_KEY_N)){
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::U_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::U);
    }

    if (glfwGetKey(window, GLFW_KEY_F)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::F_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::F);
    }

    if (glfwGetKey(window, GLFW_KEY_Y)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::B_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::B);
    }

    if (glfwGetKey(window, GLFW_KEY_M)) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            modelController->useRubiksCube(RubiksMove::D_PRIME);
        else
            modelController->useRubiksCube(RubiksMove::D);
    }

    if (glfwGetKey(window, GLFW_KEY_KP_8))
        modelController->useRubiksCube(RubiksMove::MV);
    if (glfwGetKey(window, GLFW_KEY_KP_2))
        modelController->useRubiksCube(RubiksMove::MV_PRIME);
    if (glfwGetKey(window, GLFW_KEY_KP_7))
        modelController->useRubiksCube(RubiksMove::MVS_PRIME);
    if (glfwGetKey(window, GLFW_KEY_KP_9))
        modelController->useRubiksCube(RubiksMove::MVS);
    if (glfwGetKey(window, GLFW_KEY_KP_4))
        modelController->useRubiksCube(RubiksMove::MH_PRIME);
    if (glfwGetKey(window, GLFW_KEY_KP_6))
        modelController->useRubiksCube(RubiksMove::MH);

    if (glfwGetKey(window, GLFW_KEY_KP_0))
        modelController->scrambleGenerator();
    if (glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS)
        modelController->setIsAutoSovling(true);
}

//Return digitTexture for the appropriate number
GLuint getDigit(int number)
{
    switch (number)
    {
        case 0:
            return timerTexture[0];
        case 1:
            return timerTexture[1];
        case 2:
            return timerTexture[2];
        case 3:
            return timerTexture[3];
        case 4:
            return timerTexture[4];
        case 5:
            return timerTexture[5];
        case 6:
            return timerTexture[6];
        case 7:
            return timerTexture[7];
        case 8:
            return timerTexture[8];
        case 9:
            return timerTexture[9];
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
        GLuint boxTextureID = TextureLoader::LoadTextureUsingStb("Textures/box_texture.png");
        GLuint metalTextureID = TextureLoader::LoadTextureUsingStb("Textures/metal_finish.jpg"); 
        GLuint tiledTextureID = TextureLoader::LoadTextureUsingStb("Textures/tiled_texture.png"); 
        
        //Front cubes
        GLuint x0y0z2ID = TextureLoader::LoadTextureUsingStb("Textures/x0y0z2.png");
        GLuint x0y1z2ID = TextureLoader::LoadTextureUsingStb("Textures/x0y1z2.png");
        GLuint x1y0z2ID = TextureLoader::LoadTextureUsingStb("Textures/x1y0z2.png");
        GLuint x1y1z2ID = TextureLoader::LoadTextureUsingStb("Textures/x1y1z2.png");
        GLuint x0y2z2ID = TextureLoader::LoadTextureUsingStb("Textures/x0y2z2.png");
        GLuint x1y2z2ID = TextureLoader::LoadTextureUsingStb("Textures/x1y2z2.png");
        GLuint x2y0z2ID = TextureLoader::LoadTextureUsingStb("Textures/x2y0z2.png");
        GLuint x2y1z2ID = TextureLoader::LoadTextureUsingStb("Textures/x2y1z2.png");
        GLuint x2y2z2ID = TextureLoader::LoadTextureUsingStb("Textures/x2y2z2.png");

        //Mid cubes
        GLuint x0y0z1ID = TextureLoader::LoadTextureUsingStb("Textures/x0y0z1.png");
        GLuint x1y0z1ID = TextureLoader::LoadTextureUsingStb("Textures/x1y0z1.png");
        GLuint x2y0z1ID = TextureLoader::LoadTextureUsingStb("Textures/x2y0z1.png");
        GLuint x0y1z1ID = TextureLoader::LoadTextureUsingStb("Textures/x0y1z1.png");
        GLuint x1y1z1ID = TextureLoader::LoadTextureUsingStb("Textures/x1y1z1.png");
        GLuint x2y1z1ID = TextureLoader::LoadTextureUsingStb("Textures/x2y1z1.png");
        GLuint x0y2z1ID = TextureLoader::LoadTextureUsingStb("Textures/x0y2z1.png");
        GLuint x1y2z1ID = TextureLoader::LoadTextureUsingStb("Textures/x1y2z1.png");
        GLuint x2y2z1ID = TextureLoader::LoadTextureUsingStb("Textures/x2y2z1.png");

        //Back cubes
        GLuint x0y0z0ID = TextureLoader::LoadTextureUsingStb("Textures/x0y0z0.png");
        GLuint x0y1z0ID = TextureLoader::LoadTextureUsingStb("Textures/x0y1z0.png");
        GLuint x0y2z0ID = TextureLoader::LoadTextureUsingStb("Textures/x0y2z0.png");
        GLuint x1y0z0ID = TextureLoader::LoadTextureUsingStb("Textures/x1y0z0.png");
        GLuint x1y1z0ID = TextureLoader::LoadTextureUsingStb("Textures/x1y1z0.png");
        GLuint x1y2z0ID = TextureLoader::LoadTextureUsingStb("Textures/x1y2z0.png");
        GLuint x2y0z0ID = TextureLoader::LoadTextureUsingStb("Textures/x2y0z0.png");
        GLuint x2y1z0ID = TextureLoader::LoadTextureUsingStb("Textures/x2y1z0.png");
        GLuint x2y2z0ID = TextureLoader::LoadTextureUsingStb("Textures/x2y2z0.png");

    #else
        GLuint boxTextureID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/box_texture.png"); //Source: https://jooinn.com/wood-texture-box.html
        GLuint metalTextureID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/metal_finish.jpg"); //Source: https://unsplash.com/photos/v6uiP2MD6vs
        GLuint tiledTextureID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/tiled_texture.png"); //Source: https://www.3dxo.com/textures/tiles

        //Front cubes
        GLuint x0y0z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y0z2.png");
        GLuint x0y1z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y1z2.png");
        GLuint x1y0z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y0z2.png");
        GLuint x1y1z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y1z2.png");
        GLuint x0y2z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y2z2.png");
        GLuint x1y2z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y2z2.png");
        GLuint x2y0z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y0z2.png");
        GLuint x2y1z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y1z2.png");
        GLuint x2y2z2ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y2z2.png");

        //Mid cubes
        GLuint x0y0z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y0z1.png");
        GLuint x1y0z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y0z1.png");
        GLuint x2y0z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y0z1.png");
        GLuint x0y1z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y1z1.png");
        GLuint x1y1z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y1z1.png");
        GLuint x2y1z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y1z1.png");
        GLuint x0y2z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y2z1.png");
        GLuint x1y2z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y2z1.png");
        GLuint x2y2z1ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y2z1.png");

        //Back cubes
        GLuint x0y0z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y0z0.png");
        GLuint x0y1z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y1z0.png");
        GLuint x0y2z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x0y2z0.png");
        GLuint x1y0z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y0z0.png");
        GLuint x1y1z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y1z0.png");
        GLuint x1y2z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x1y2z0.png");
        GLuint x2y0z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y0z0.png");
        GLuint x2y1z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y1z0.png");
        GLuint x2y2z0ID = TextureLoader::LoadTextureUsingStb("../Assets/Textures/x2y2z0.png");
        
        GLuint digit0 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit0.png");
        GLuint digit1 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit1.png");
        GLuint digit2 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit2.png");
        GLuint digit3 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit3.png");
        GLuint digit4 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit4.png");
        GLuint digit5 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit5.png");
        GLuint digit6 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit6.png");
        GLuint digit7 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit7.png");
        GLuint digit8 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit8.png");
        GLuint digit9 = TextureLoader::LoadTextureUsingStb("../Assets/Textures/digit9.png");

    #endif

    //Array of textures
    textureArray[0] = toggle; // 0 (texture off) or 1 (texture on)
    textureArray[1] = boxTextureID;
    textureArray[2] = metalTextureID;
    textureArray[3] = tiledTextureID;
    textureArray[4] = x0y0z0ID; //start rubix textures
    textureArray[5] = x0y0z1ID;
    textureArray[6] = x0y0z2ID;
    textureArray[7] = x0y1z0ID;
    textureArray[8] = x0y1z1ID;
    textureArray[9] = x0y1z2ID;
    textureArray[10] = x0y2z0ID;
    textureArray[11] = x0y2z1ID;
    textureArray[12] = x0y2z2ID;
    textureArray[13] = x1y0z0ID;
    textureArray[14] = x1y0z1ID;
    textureArray[15] = x1y0z2ID;
    textureArray[16] = x1y1z0ID;
    textureArray[17] = x1y1z1ID;
    textureArray[18] = x1y1z2ID;
    textureArray[19] = x1y2z0ID;
    textureArray[20] = x1y2z1ID;
    textureArray[21] = x1y2z2ID;
    textureArray[22] = x2y0z0ID;
    textureArray[23] = x2y0z1ID;
    textureArray[24] = x2y0z2ID;
    textureArray[25] = x2y1z0ID;
    textureArray[26] = x2y1z1ID;
    textureArray[27] = x2y1z2ID;
    textureArray[28] = x2y2z0ID;
    textureArray[29] = x2y2z1ID;
    textureArray[30] = x2y2z2ID;


    //Timer texture
    timerTexture[0] = digit0;
    timerTexture[1] = digit1;
    timerTexture[2] = digit2;
    timerTexture[3] = digit3;
    timerTexture[4] = digit4;
    timerTexture[5] = digit5;
    timerTexture[6] = digit6;
    timerTexture[7] = digit7;
    timerTexture[8] = digit8;
    timerTexture[9] = digit9;

    // Black background
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

    //Load shaders 
    #if defined(PLATFORM_OSX)
        std::string shaderPathPrefix = "Shaders/";
    #else
        std::string shaderPathPrefix = "../Assets/Shaders/";
    #endif

    int shaderProgram = loadSHADER(shaderPathPrefix + "color_vertex.glsl", shaderPathPrefix + "color_fragment.glsl");
    int texturedShaderProgram = loadSHADER(shaderPathPrefix + "texture_vertex.glsl", shaderPathPrefix + "texture_fragment.glsl");
    depthShaderProgram = loadSHADER(shaderPathPrefix + "depth_vertex.glsl", shaderPathPrefix + "depth_fragment.glsl");

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

    //Initialize model sphere object
    Sphere sphere(5.5f, 100, 50, shaderProgram, vec3(0.0f, 8.0f, 0.0f));
    sphere.buildSphere(); //Generate all the vertices to send to gpu

    glBindVertexArray(vaoArray[6]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[6]);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVerticesSize(), sphere.getVertices(), GL_STATIC_DRAW);

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


    glfwSetWindowSizeCallback(window, framebuffer_size_callback); //Handle window resizing
    glfwSetCursorEnterCallback(window, cursor_enter_callback); //Handle cursor leaving window event: Stop tracking mouse mouvement
    glfwSetCursorPosCallback(window, cursor_position_callback); //Handle cursor mouvement event: Update ViewController's mouse position
    
    viewController->initCamera();

    // Toggle Shadow on/off
    bool toggleShadow = false;
    // timer
  	bool timer = false;

    int timeElapsed; //glfwGetTime() starts when the GLFW gets initialized, so we need the time elapsed between the initialization of GLFW and the moment user presses KP_0

     // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        //Timer
	if (timer == true)
	{
	    system("CLS");
	    //std::cout << floor(glfwGetTime()) << std::endl;

        int ones = ((int)floor(glfwGetTime()) - timeElapsed) % 10; //Calculate ones digit
        textureArray[31] = getDigit(ones);
        int tens = (((int)floor(glfwGetTime()) - timeElapsed) /10)%10; //Calculate tens digit
        textureArray[32] = getDigit(tens);
        int hundreds = (((int)floor(glfwGetTime()) - timeElapsed) /100) % 10; //Calculate hundres digit
        textureArray[33] = getDigit(hundreds);
        int thousands = (((int)floor(glfwGetTime()) - timeElapsed) / 1000) % 10; //Calculate hundres digit
        textureArray[34] = getDigit(thousands);
    
	}
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
	{
       timeElapsed = (int)floor(glfwGetTime());
       timer = true;

	   //if (timer == true)
	   //{
	   //   system("CLS");
	   //   std::cout << glfwGetTime() << std::endl;
	   //   timer = false;

	   //}
	   //else
	   //{
	   //   timer = true;
	   //}
	}


        worldRotationX = rotate(glm::mat4(1.0f), glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        worldRotationY = rotate(glm::mat4(1.0f), glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        worldRotationUpdate = worldRotationX * worldRotationY;

        //Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        viewController->updateDt(dt);
        modelController->setDt(dt);
  
        //Get user inputs
        updateInput(window, dt, worldRotation, shaderArray);
        modelController->automatedCubeAction();

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
                glProgramUniformMatrix4fv(depthShaderProgram, glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
                toggleShadow = false;
            }
            else
            {
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

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}
