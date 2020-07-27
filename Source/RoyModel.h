#pragma once
#include "Model.h"
class RoyModel :public Model{
public:
	RoyModel() :Model() {};
	RoyModel(vec3 position, float size) :Model(position, size) {};
	RoyModel(const Model& model) :Model(model) {};
	virtual ~RoyModel();
	virtual void draw(mat4 worldRotationUpdate);
	virtual void drawLetter(mat4 worldRotationUpdate);
	virtual void drawDigit(mat4 worldRotationUpdate);
};

