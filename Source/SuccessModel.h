#pragma once
#include "Model.h"
#include "Sphere.h"
class SuccessModel : public Model {
public:
	SuccessModel() :Model() {};
	SuccessModel(vec3 position, float size) :Model(position, size) {};
	SuccessModel(const Model& model) :Model(model) {};
	virtual ~SuccessModel();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
	virtual void drawLetter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter2(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter3(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter4(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter5(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter6(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawLetter7(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
};