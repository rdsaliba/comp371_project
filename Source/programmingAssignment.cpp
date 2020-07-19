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

using namespace std; 
using namespace glm;

const GLuint WIDTH = 1024, HEIGHT = 768;
glm::mat4 projection_matrix;

float gridUnit = 1.0f;
void modelFocusSwitch(int nextModel);
int SELECTEDMODELINDEX = 1;
Model* focusedModel = NULL;
Model models[] = {
    Model(vec3(0.0f, 0.0f, 0.0f), 0.0f), //axis lines
    TaqiModel(vec3(-45.0f, 0.0f, -45.0f), 0.0f), //Taqi (Q4)
    HauModel(vec3(45.0f, 0.0f, -45.0f), 0.0f), //Hau (U6)
    RoyModel(vec3(-45.0f, 0.0f, 45.0f), 0.0f), //Roy (Y8)    
    SwetangModel(vec3(0.0f, 0.0f, 0.0f), 0.0f), //Swetang (E0) 
    Model(vec3(45.0f, 0.0f, 45.0f), 0.0f) //William (L9) 
};

//Default
const glm::vec3 eye(0.0f, 7.0f, 20.0f);
const glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);
//Camera settings
glm::vec3 centerO = center;
glm::vec3 cameraEye = eye;
float x_rotate = 0;
float y_rotate = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//update projection matrix to new width and height
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
}
const char* getVertexShaderSource()
{
	//Vertex Shader
	return
		"#version 330 core\n"
		"layout (location=0) in vec3 aPos;"
		"layout (location=1) in vec3 aColor;"
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
	//Fragment Shaders here 
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

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}


vec3 cubeVertexArray[] = {  
    // position,                   color
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
	glBindVertexArray(vao[4]);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

	Model model = models[5];
	mat4 rotationUpdate = rotate(glm::mat4(1.0f), radians(model.getRotation().y), vec3(0.0f, 1.0f, 0.0f));
	mat4 scaleUpdate = scale(glm::mat4(1.0f), glm::vec3(1.0f + model.getScaling(), 1.0f + model.getScaling(), 1.0f + model.getScaling()));

	
	//groupMatrix will be applied to all of the cubes for this model (will translate the complete model)
	mat4 groupMatrix = worldRotationUpdate * translate(glm::mat4(1.0f), model.getPosition()) * rotationUpdate * scaleUpdate; //Translate model to upper left corner

	//Letter L (verticle line)
	mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.1f, 0.5f, 1.0f));
	mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
	mat4 rotationMatrix = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	mat4 partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	mat4 worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);

	//Letter L (bottom)
	scaleMatrix = scale(glm::mat4(1.0f), glm::vec3(1.5f, 0.5f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
	partMatrix = translationMatrix * scaleMatrix;
	worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);

	//NUMBER 9 (bottom)

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.5f, 0.0f));
	partMatrix = translationMatrix * scaleMatrix;
	worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);

	//NUMBER 9 (top)
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 1.5f, 0.0f));
	partMatrix = translationMatrix * scaleMatrix;
	worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);


	//Left side of 9
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.0f));
	rotationMatrix = rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);


	//Right side of 9
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.2f, 0.5f, 1.0f));
	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.2f, 0.6f, 0.0f));
	partMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	worldMatrix = groupMatrix * partMatrix;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
	glDrawArrays(model.getRenderMode(), 0, 36);
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

    // Camera parameters for view transform
    //Initial view values
    vec3 cameraPosition(0.0f, 5.0f, 10.0f);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    models[1].setVbo(vboArray[4]);
    models[2].setVbo(vboArray[4]);
    models[3].setVbo(vboArray[4]);
    models[4].setVbo(vboArray[4]);
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
    //glEnable(GL_DEPTH_TEST);

    mat4 worldRotationX;
    mat4 worldRotationY;
    mat4 worldRotationUpdate;

     // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        worldRotationX = rotate(glm::mat4(1.0f), glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        worldRotationY = rotate(glm::mat4(1.0f), glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        worldRotationUpdate = worldRotationX * worldRotationY;

        //Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        //Get user inputs
        updateInput(window, dt, worldRotation);

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        drawGroundGrid(shaderProgram, vaoArray, gridUnit, worldRotationUpdate);
        axis.drawAxisLines(shaderProgram, vaoArray, gridUnit, worldRotationUpdate);

        //MODELS
        drawTaqiModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawHauModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawRoyModel(shaderProgram, vaoArray, worldRotationUpdate);
        drawSwetangModel(shaderProgram, vaoArray, worldRotationUpdate);
        //drawWilliamModel(shaderProgram, vaoArray, worldRotationUpdate);

        //FPS camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS; //Press shift to go faster
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

        // - Calculate mouse motion dx and dy
        // - Update camera horizontal and vertical angle
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 5.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector); //vector to be normalized

        // Use camera lookat and side vectors to update positions with CVBG
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            cameraPosition += cameraLookAt * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        {
            cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            cameraPosition += cameraSideVector * dt * currentCameraSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
        }

        // Set the view matrix for first person camera
        mat4 viewMatrix(1.0f);
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        setViewMatrix(shaderProgram, viewMatrix);

        //Mouse Panning, Tilting and Zooming
        int pan = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        int tilt = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
        int zoom = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        if (pan == GLFW_PRESS)
        {
            cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
            vec3 eyeHorizon(cameraHorizontalAngle, 0.0f, 0.0f);

            viewMatrix = lookAt(eye + eyeHorizon, center + eyeHorizon, up);
            setViewMatrix(shaderProgram, viewMatrix);
        }
        if (tilt == GLFW_PRESS)
        {
            cameraVerticalAngle -= dy * cameraAngularSpeed * dt;
            vec3 eyeVertical(0.0f, cameraVerticalAngle, 0.0f);
            viewMatrix = lookAt(eye + eyeVertical, center, up);
            setViewMatrix(shaderProgram, viewMatrix);
        }
        if (zoom == GLFW_PRESS)
        {
            cameraVerticalAngle -= dy * cameraAngularSpeed * dt;
            vec3 eyeVertical(0.0f, 0.0f, cameraVerticalAngle);
            viewMatrix = lookAt(eye + eyeVertical, center, up);
            setViewMatrix(shaderProgram, viewMatrix);
        }

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
