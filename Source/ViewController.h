#pragma once
#define GLEW_STATIC // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <algorithm>
#include "Camera.h"
#include "Light.h"
using namespace glm;
class ViewController {
public:

	ViewController();
	ViewController(GLFWwindow* window, int width, int height, int shaderProgram, int shaderArray[], Light* lights[]);
	~ViewController();

	void setShaderProgram(int shaderProgram) { this->shaderProgram = shaderProgram; }
	void setViewMatrix(int shaderProgram);
	void setFastCam(bool isFastCam) { this->fastCam = isFastCam; }
	void setLastMousePosX(double lastMousePosX) { this->lastMousePosX = lastMousePosX; }
	void setLastMousePosY(double lastMousePosY) { this->lastMousePosY = lastMousePosY; }
	void setMousePosX(double mousePosX) { this->mousePosX = mousePosX; }
	void setMousePosY(double mousePosY) { this->mousePosY = mousePosY; }

    double getLastMousePosX() { return this->lastMousePosX; }
    double getLastMousePosY() { return this->lastMousePosY; }

	void initCamera(Camera* camera, Light* light);
	void setView(Camera* camera, Light* light);
	void updateDt(float dt) { this->dt = dt; }
	void update(int shaderType, Light* light);

private:
	int width;
	int height;
	int shaderProgram;
	int shaderArray[3];
	GLFWwindow* window;
	mat4 projection_matrix;
	mat4 viewMatrix;

	//Frame time
	float dt;

	//Cursor position
	double lastMousePosX;
	double lastMousePosY;
	double mousePosX;
	double mousePosY;
	bool fastCam;
	
	Camera* cam;
	Light* lights[2];

	// Camera parameters for view transform
	vec3 cameraPosition;
	vec3 cameraLookAt;
	vec3 cameraUp;

	// Other camera parameters
	float cameraSpeed = 1.0f;
	float cameraHorizontalAngle = 90.0f;
	float cameraVerticalAngle = 0.0f;
};

