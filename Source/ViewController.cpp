#include "ViewController.h"

ViewController::ViewController() {
	this->width = 1024;
	this->height = 768;
    this->projection_matrix = mat4(1.0f);
	this->shaderProgram = -1;
    this->shaderArray[0] = -1;
    this->shaderArray[1] = -1;

	this->fastCam = false;
	this->cameraSpeed = 1.0f;
	this->cameraFastSpeed = 2 * cameraSpeed;
	this->cameraHorizontalAngle = 90.0f;
	this->cameraVerticalAngle = 0.0f;

	this->cameraPosition = vec3(0.0f, 10.0f, 20.0f);
	this->cameraLookAt = vec3(0.0f, 0.0f, 0.0f);
	this->cameraUp = vec3(0.0f, 1.0f, 0.0f);

    this->window = NULL;
    this->dt = 0;
    this->lastMousePosX = 0;
    this->lastMousePosY = 0;
    this->mousePosX = 0;
    this->mousePosY = 0;
    this->viewMatrix = mat4(1.0f);
}

ViewController::ViewController(GLFWwindow* window, int width, int height, int shaderProgram, int shaderArray[]) {
	this->window = window;
	this->width = width;
	this->height = height;
	this->shaderProgram = shaderProgram;
    this->shaderArray[0] = shaderArray[0];
    this->shaderArray[1] = shaderArray[1];
    this->projection_matrix = mat4(1.0f);
	
    this->fastCam = false;
    this->cameraSpeed = 1.0f;
    this->cameraFastSpeed = 2 * cameraSpeed;
    this->cameraHorizontalAngle = 90.0f;
    this->cameraVerticalAngle = 0.0f;

    this->cameraPosition = vec3(0.0f, 10.0f, 20.0f);
    this->cameraLookAt = vec3(0.0f, 0.0f, 0.0f);
    this->cameraUp = vec3(0.0f, 1.0f, 0.0f);

    this->dt = 0;
    this->lastMousePosX = 0;
    this->lastMousePosY = 0;
    this->mousePosX = 0;
    this->mousePosY = 0;
    this->viewMatrix = mat4(1.0f);
}

ViewController::~ViewController() {
	window = NULL;
}

/// <summary>
/// Initialize Cursos position, view matrix, and projection matrix to default (center screen, looking at origin)
/// </summary>
void ViewController::initCamera() {
    //Set Projection
    this->projection_matrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection_matrix[0][0]);

    //Start application with cursor in the center of the window
    mousePosX = width / 2.0;
    mousePosY = height / 2.0;
    glfwSetCursorPos(window, mousePosX, mousePosY);

    // Set initial view matrix
    viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUp);
    setViewMatrix(shaderArray[0]);
    setViewMatrix(shaderArray[1]);
}

void ViewController::setViewMatrix(int shaderType){
	glUseProgram(shaderType);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderType, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

/// <summary>
/// Update View/camera according to user inputs
/// </summary>
void ViewController::update(int shaderType) {
	float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

    // - Calculate mouse motion dx and dy
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

    normalize(cameraSideVector); //vector to be normalized

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
    viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
    
    //
    glUseProgram(shaderProgram);
    GLuint vP = glGetUniformLocation(shaderProgram, "viewPos");
    
    glUniformMatrix4fv(vP, 1, GL_FALSE, &cameraPosition[0]);
    
    //Mouse Panning, Tilting and Zooming
    int pan = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int tilt = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    int zoom = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (pan == GLFW_PRESS)
    {
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        vec3 eyeHorizon(cameraHorizontalAngle, 0.0f, 0.0f);
        viewMatrix = lookAt(cameraPosition + eyeHorizon, cameraLookAt + eyeHorizon, cameraUp);
    }
    if (tilt == GLFW_PRESS)
    {
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;
        vec3 eyeVertical(0.0f, cameraVerticalAngle, 0.0f);
        viewMatrix = lookAt(cameraPosition + eyeVertical, cameraLookAt, cameraUp);
    }
    if (zoom == GLFW_PRESS)
    {
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;
        vec3 eyeVertical(0.0f, 0.0f, cameraVerticalAngle);
        viewMatrix = lookAt(cameraPosition + eyeVertical, cameraLookAt, cameraUp);
    }
    setViewMatrix(shaderType);
}
