#include "Model.h"

Model::Model() {
	this->position = vec3(0.0f, 0.0f, 0.0f);
	this->scaling = 0.0f;
	this->rotation = vec3(1.0f, 1.0f, 1.0f);
	this->renderMode = GL_TRIANGLES;
}

Model::Model(vec3 position, float scaling) {
	this->position = position;
	this->scaling = scaling;
	this->rotation = vec3(1.0f, 1.0f, 1.0f);
	this->renderMode = GL_TRIANGLES;
}

void Model::updatePosition(vec3 moveVector) {
	this->position.x += moveVector.x;
	this->position.y += moveVector.y;
	this->position.z += moveVector.z;
}
