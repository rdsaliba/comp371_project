#pragma once
#include "Model.h"
class SwetangModel : public Model {
public:
	SwetangModel() :Model() {};
	SwetangModel(vec3 position, float size) :Model(position, size) {};
	SwetangModel(const Model& model) :Model(model) {};
	virtual ~SwetangModel();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
	virtual void drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawDigit(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
};