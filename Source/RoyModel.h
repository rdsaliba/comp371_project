#pragma once
#include "Model.h"
class RoyModel :public Model{
public:
	RoyModel() :Model() {};
	RoyModel(vec3 position, float size) :Model(position, size) {};
	RoyModel(const Model& model) :Model(model) {};
	virtual ~RoyModel();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
	virtual void drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
};

