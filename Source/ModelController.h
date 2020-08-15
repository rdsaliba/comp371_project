#pragma once
#include <vector>
#include "Model.h"
#include "HauModel.h"
#include "RoyModel.h"
#include "TaqiModel.h"
#include "SwetangModel.h"
#include "WilliamModel.h"
#include "ModelUtilities.h"
#include "RubiksModel.h"
using namespace std;
class ModelController {
public:
	Model* focusedModel;
	
	ModelController();
	~ModelController();

	float getDt() { return dt; }
	void addModel(Model* model);
	void initModels(int shaderProgram, unsigned int vao, unsigned int vbo, Sphere sphere);
	void drawModels(mat4 worldRotationUpdate, GLuint textureArray[], int shaderProgram);
	void modelFocusSwitch(int nextModel);
	bool getIsScrambling() { return isScrambling; }
	bool getIsAutoSolving() { return isAutoSolving; }
	RubiksMove getReverseMove(RubiksMove move);

	void setModelsShaderProgram(int shaderProgram);
	void setModelsVAO(unsigned int vao);
	void setModelsVBO(unsigned int vbo);
	void setModelsSphere(Sphere sphere);
	void setDt(float dt) { this->dt = dt; }
	void setIsScrambling(bool isScrambling) { this->isScrambling = isScrambling; }
	void setIsAutoSovling(bool isAutoSolving) { this->isAutoSolving = isAutoSolving; }

	//Functions to update the currently selected model
	void updateScaling(float scaling) { focusedModel->updateScaling(scaling); }
	void updateRenderMode(GLenum renderMode) { focusedModel->setRenderMode(renderMode); }
	void updateX(float x) { focusedModel->x(x); }
	void updateY(float y) { focusedModel->y(y); }
	void updateZ(float z) { focusedModel->z(z); }
	void updateRotationY(float rotation) { focusedModel->updateRotationY(rotation); }
	void updateShearingY(float shear) { focusedModel->updateShearingY(shear); }
	void randomPosition(vec3 value);
	
	//Rubiks cube manipulation functions
	void useRubiksCube(RubiksMove move);
	void scrambleGenerator();
	void scramble();
	void solve();
	void automatedCubeAction();
private:
	vector<Model*> models;
	RubiksModel* rubiksCube;
	int selectedModelIndex;
	float dt;
	bool isScrambling;
	bool isAutoSolving;
	vector<RubiksMove> scrambleList;
	vector<RubiksMove> solveList;
};