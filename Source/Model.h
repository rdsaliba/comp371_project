#ifndef MODEL_H
#define MODEL_H
#include <glm/glm.hpp>
#include <GL/glew.h> 
using namespace glm;
/// <summary>
/// Movable 3D model location/transformation information
/// @author Hau Gilles Che 
/// </summary>
class Model {
private:
	//Location of model in 3D world
	vec3 position;
	//Scaling value applied onto model to change base size
	float scaling;
	//Orientation of model in 3D world
	vec3 rotation;
	
	GLenum renderMode;

public:
	Model();
	Model(vec3 position, float size);

	vec3 getPosition() { return position; }
	float getScaling() { return scaling; }
	vec3 getRotation() { return rotation; }
	GLenum getRenderMode() { return renderMode; }

	void setPosition(vec3 position) { this->position = position; }
	void setSize(float scaling) { this->scaling = scaling; }
	void setRotation(vec3 rotation) { this->rotation = rotation; }
	void setRenderMode(GLenum renderMode) { this->renderMode = renderMode; }

	void updatePosition(vec3 moveVector);
	void updateScaling(float value) { this->scaling += value; }

	//Translation values
	void x(float value) { this->position.x += value; }
	void y(float value) { this->position.y += value; }
	void z(float value) { this->position.z += value; }

	void updateRotationY(float yValue) { this->rotation.y += yValue; }
};

#endif // !MODEL_H
