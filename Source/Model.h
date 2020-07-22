#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace glm;
/// <summary>
/// Movable 3D model location/transformation information
/// @author Hau Gilles Che 
/// </summary>
class Model {
public:
	Model();
	Model(vec3 position, float size);
	Model(const Model& model);
	~Model();

	vec3 getPosition() { return position; }
	float getScaling() { return scaling; }
	vec3 getRotation() { return rotation; }
	GLenum getRenderMode() { return renderMode; }
	int getShaderProgram() { return shaderProgram; }
	int getToggle() { return toggle; }

	void setPosition(vec3 position) { this->position = position; }
	void setSize(float scaling) { this->scaling = scaling; }
	void setRotation(vec3 rotation) { this->rotation = rotation; }
	void setRenderMode(GLenum renderMode) { this->renderMode = renderMode; }
	void setShaderProgram(int shaderProgram) { this->shaderProgram = shaderProgram; }
	void setVao(GLuint vao) { this->vao = vao; }
	void setVbo(GLuint vbo) { this->vbo = vbo; }
	void setToggle(int toggleMode) { this->toggle = toggleMode; }
	

	void setTexture(GLuint toggle, GLuint texture)
	{
		if (toggle == 1)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
		}
	}
	void updatePosition(vec3 moveVector);
	void updateScaling(float value) { this->scaling += value; }

	//Translation values
	void x(float value) { this->position.x += value; }
	void y(float value) { this->position.y += value; }
	void z(float value) { this->position.z += value; }

	void updateRotationY(float yValue) { this->rotation.y += yValue; }

	virtual void draw(mat4 model);
	//virtual void drawLetter(mat4 worldRotationUpdate);
	//virtual void drawDigit(mat4 worldRotationUpdate);
protected:
	void drawPart(mat4 worldRotationUpdate, mat4 partMatrix, vec3 componentPosition);
	//Location of model in 3D world
	vec3 position;
	//Scaling value applied onto model to change base size
	float scaling;
	//Orientation of model in 3D world
	vec3 rotation;

	GLenum renderMode;
	int toggle;

	unsigned int vao;
	unsigned int vbo;
	int shaderProgram;

};
