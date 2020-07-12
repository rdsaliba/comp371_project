//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include "Model.h"

using namespace glm;
using namespace std;

void modelFocusSwitch(int nextModel);
GLenum renderMode = GL_TRIANGLES;
int SELECTEDMODELINDEX = 1;
Model* focusedModel = NULL;
Model models[] = {
    Model(vec3(0.0f, 0.0f, 0.0f), 0.0f), //axis lines
    Model(vec3(-0.6f, 2.5f, 0.0f), 0.0f), //Taqi (Q4)
    Model(vec3(2.0f, 2.0f, -2.0f), 0.0f) //Hau (U6)
        
};



const char* getVertexShaderSource()
{
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);" //default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}


const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
        "}";
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

float gridUnit = 1.0f;

// Cube model
vec3 cubeVertexArray[] = {  // position,                            color
    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
    vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
};

vec3 gridVertexArray[] = {
    //ground unit square vertices
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(gridUnit, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(gridUnit, 0.0f, gridUnit),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, gridUnit),
    glm::vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(5 * gridUnit, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),


    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 5 * gridUnit,0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 5 * gridUnit),
    glm::vec3(0.0f, 0.0f, 1.0f)
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

GLuint createVertexArrayObject(vec3* vertexArray)
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    //glBindVertexArray(vao);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(glm::vec3),
        (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}



/// <summary>
/// draws 1 ground square
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="xDisplacement"></param>
/// <param name="yDisplacement"></param>
/// <param name="zDisplacement"></param>
void drawGridSquare(GLuint worldMatrixLocation, float xDisplacement, float yDisplacement, float zDisplacement, float gridUnit) {
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xDisplacement, yDisplacement, zDisplacement));
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/// <summary>
/// Generates ground grid
/// </summary>
/// <param name="worldMatrixLocation"></param>
/// <param name="pointDisplacementUnit">Length of vertex</param>
void drawGroundGrid(int shader, GLuint vao[], float pointDisplacementUnit) {
    glBindVertexArray(vao[0]);
    mat4 worldMatrix = mat4(1.0f);
    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    for (int row = -50; row < 50; row++) {
        for (int col = -50; col < 50; col++) {
            drawGridSquare(worldMatrixLocation, col * pointDisplacementUnit, 0.0f, row * pointDisplacementUnit, pointDisplacementUnit);
        }
    }
}

void drawAxisLines(int shader, GLuint vao[], float gridUnit) {
    glBindVertexArray(vao[1]);
    mat4 axisMatrix = mat4(1.0f);
    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    //X-axis
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Y-axis
    glBindVertexArray(vao[2]);
    axisMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Z-axis
    glBindVertexArray(vao[3]);
    axisMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);
}

/// <summary>
/// Draws Hau's model (U6)
/// </summary>
/// <param name="shader"></param>
/// <param name="vao"></param>
void drawHauModel(int shader, GLuint vao[]) {
    glBindVertexArray(vao[4]);

    Model model = models[2];
    mat4 rotationUpdate = rotate(glm::mat4(1.0f), radians(1.0f + model.getRotation().y), vec3(0.0f, 1.0f, 0.0f));
    mat4 scaleUpdate = scale(glm::mat4(1.0f), vec3(1.0f + model.getScaling(), 1.0f + model.getScaling(), 1.0f + model.getScaling()));
       
    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    mat4 projectionMatrix = perspective(70.0f, 1024.0f / 768.0f, 0.01f, 100.0f);

    GLuint projectionMatrixLocation = glGetUniformLocation(shader, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    mat4 scaleMatrix = scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
    mat4 groupMatrix = translate(mat4(1.0f), model.getPosition()) * rotationUpdate * scaleUpdate;

    //U-Bottom 
    mat4 translationMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
    mat4 partMatrix = translationMatrix * scale(mat4(1.0f), vec3(2.0f, 1.0f, 1.0f));
    mat4 uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //U-Left
    translationMatrix = translate(mat4(1.0f), vec3(-1.0f, 2.0f, 0.0f));
    mat4 rotationMatrix = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    uMatrix = partMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);
    
    //U-Right
    translationMatrix = translate(mat4(1.0f), vec3(1.0f, 2.0f, 0.0f));
    partMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    //6-bottom
    translationMatrix = translate(mat4(1.0f), vec3(4.5f, 0.0f, 0.0f));
    partMatrix = translationMatrix * scale(mat4(1.0f), vec3(2.0f, 1.0f, 1.0f));
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    //6-left
    translationMatrix = translate(mat4(1.0f), vec3(3.0f, 2.0f, 0.0f));
    partMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    //6-right
    translationMatrix = translate(mat4(1.0f), vec3(5.5f, 1.0f, 0.0f));
    partMatrix = translationMatrix * scale(mat4(1.0f), vec3(1.0f, 3.0f, 1.0f)) * rotationMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    //6-top loop
    translationMatrix = translate(mat4(1.0f), vec3(4.5f, 2.0f, 0.0f));
    partMatrix = translationMatrix * scale(mat4(1.0f), vec3(2.0f, 1.0f, 1.0f)) * rotationMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    //6-top branch
    translationMatrix = translate(mat4(1.0f), vec3(4.5f, 4.0f, 0.0f));
    partMatrix = translationMatrix * scale(mat4(1.0f), vec3(3.0f, 1.0f, 1.0f)) * rotationMatrix;
    uMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &uMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);
}


//TAQI'S MODEL ("Q4")
void drawTaqiModel(int shaderProgram, GLuint vao[])
{
    glBindVertexArray(vao[4]);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

    Model model = models[1];
    mat4 rotationUpdate = rotate(glm::mat4(1.0f), radians(1.0f + model.getRotation().y), vec3(0.0f, 1.0f, 0.0f));
    mat4 scaleUpdate = scale(glm::mat4(1.0f), glm::vec3(1.0f + model.getScaling(), 1.0f + model.getScaling(), 1.0f + model.getScaling()));

    //Taqi's Model
    //Cube scale (for most of the cubes)
    mat4 scaleMatrix = scale(glm::mat4(1.0f), glm::vec3(1.5f, 0.5f, 1.0f));

    //groupMatrix will be applied to all of the cubes for this model (will translate the complete model)
    mat4 groupMatrix = translate(glm::mat4(1.0f), model.getPosition()) * rotationUpdate * scaleUpdate; //Translate model to upper left corner

    //LETTER Q (bottom)
    mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    mat4 partMatrix = translationMatrix * scaleMatrix;
    mat4 worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);

    
    //Left side of Q
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 1.0f, 0.0f));
    mat4 rotationMatrix = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);



    //Top of Q
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f));
    partMatrix = translationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //Right side of Q
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //Q tail
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));
    rotationMatrix = rotate(glm::mat4(1.0f), glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //NUMBER 4 (bottom)
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.5f, 0.0f));
    partMatrix = translationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //Left side of 4
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
    rotationMatrix = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);


    //Right side of 4
    scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.2f, 0.5f, 1.0f));
    translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.2f, 0.7f, 0.0f));
    partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    worldMatrix = groupMatrix * partMatrix;

    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    glDrawArrays(renderMode, 0, 36);
}

//Update through user input
void updateInput(GLFWwindow* window)
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

    //rotation (r and t)
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        (*focusedModel).updateRotationY(-5.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        (*focusedModel).updateRotationY(5.0f);
    }

    //Rendering mode
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        renderMode = GL_POINTS;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        renderMode = GL_LINES;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        renderMode = GL_TRIANGLES;
    }

    //Focused model
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        modelFocusSwitch(1);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        modelFocusSwitch(2);
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
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Project", NULL, NULL);
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

    // Camera parameters for view transform
    vec3 cameraPosition(0.0f, 5.0f, 20.0f);
    vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;

    glm::mat4 projectionMatrix = glm::perspective(70.0f, 1024.0f / 768.0f, 0.01f, 100.0f);

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        vec3(0.0f, 0.0f, 0.0f),  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    const int modelCount = 5; //number of models to load
    GLuint vaoArray[modelCount], vboArray[modelCount];
    glGenVertexArrays(modelCount, &vaoArray[0]);
    glGenBuffers(modelCount, &vboArray[0]);

    // Define and upload geometry to the GPU here ...

    //Ground Grid
    glBindVertexArray(vaoArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertexArray), gridVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Axis
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

    //Cube
    glBindVertexArray(vaoArray[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vboArray[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexArray), cubeVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);


    // Variables to be used later in tutorial
    float angle = 0;
    float rotationSpeed = 180.0f;  // 180 degrees per second
    float lastFrameTime = glfwGetTime();

    focusedModel = &models[1];
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
     // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        //Get user inputs
        updateInput(window);

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawGroundGrid(shaderProgram, vaoArray, gridUnit);
        drawAxisLines(shaderProgram, vaoArray, gridUnit);

        //MODELS
        drawTaqiModel(shaderProgram, vaoArray);
        drawHauModel(shaderProgram, vaoArray);

        //glBindVertexArray(0); 
        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Projection Transform
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.10f, 100.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            glm::mat4 projectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -100.0f, 100.0f);

            GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
            glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        }
    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}
