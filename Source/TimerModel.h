#pragma once
#include "Model.h"
class TimerModel :public Model {
public:
	TimerModel() :Model() {};
	TimerModel(vec3 position, float size) :Model(position, size) {};
	TimerModel(const Model& model) :Model(model) {};
	virtual ~TimerModel();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
	virtual void drawTimerLeft(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawTimerCenter(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);
	virtual void drawTimerRight(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);

};
