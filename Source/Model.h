#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Sphere.h"
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
	vec3 getShearing() { return shearYZ; }
	GLenum getRenderMode() { return renderMode; }
	int getShaderProgram() { return shaderProgram; }

	void setPosition(vec3 position);
	void setSize(float scaling) { this->scaling = scaling; }
	void setRotation(vec3 rotation) { this->rotation = rotation; }
	void setShear(vec3 shearYZ) { this->shearYZ = shearYZ; }
	void setRenderMode(GLenum renderMode) { this->renderMode = renderMode; }
	void setShaderProgram(int shaderProgram) { this->shaderProgram = shaderProgram; }
	void setVao(GLuint vao) { this->vao = vao; }
	void setVbo(GLuint vbo) { this->vbo = vbo; }
	void setSphere(Sphere sphere);

	void setTexture(GLuint toggle, GLuint texture)
	{
		if (toggle == 1)
		{
			glBindTexture(GL_TEXTURE_2D, texture);
		}
	}

	void updatePosition(vec3 moveVector);
	void updateScaling(float value);

	//Translation values - Updates the position of a model and its depencies
	void x(float value);
	void y(float value);
	void z(float value);

	void updateRotationY(float yValue) { this->rotation.y += yValue; }

	void updateShearingY(float value) { this->shearYZ.z += value; }

	virtual void draw(mat4 model, GLuint textureArray[]);
protected:
	void drawPart(mat4 worldRotationUpdate, mat4 partMatrix, vec3 componentPosition);
	void drawSphere(mat4 worldRotationUpdate);
	//Location of model in 3D world
	vec3 position;
	//Scaling value applied onto model to change base size
	float scaling;
	//Orientation of model in 3D world
	vec3 rotation;

	vec3 shearYZ;

	GLenum renderMode;
	unsigned int vao;
	unsigned int vbo;
	int shaderProgram;
	Sphere sphere;
};
