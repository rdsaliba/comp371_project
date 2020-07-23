#pragma once
#include <list>
#include "Model.h"
#include "HauModel.h"
#include "RoyModel.h"
#include "TaqiModel.h"
#include "SwetangModel.h"
#include "WilliamModel.h"
#include <vector>

class ModelController {
public:
	Model* focusedModel;
	
	ModelController();
	~ModelController();

	void addModel(Model* model);
	void initModels(int shaderProgram, unsigned int vao, unsigned int vbo);
	void drawModels(mat4 worldRotationUpdate);
	void modelFocusSwitch(int nextModel);
	void setModelsShaderProgram(int shaderProgram);
	void setModelsVAO(unsigned int vao);
	void setModelsVBO(unsigned int vbo);

	//Functions to update the currently selected model
	void updateScaling(float scaling) { focusedModel->updateScaling(scaling); }
	void updateRenderMode(GLenum renderMode) { focusedModel->setRenderMode(renderMode); }
	void updateX(float x) { focusedModel->x(x); }
	void updateY(float y) { focusedModel->y(y); }
	void updateRotationY(float rotation) { focusedModel->updateRotationY(rotation); }
private:
	vector<Model*> models;
	int selectedModelIndex; 
};