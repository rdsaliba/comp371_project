#include "ModelController.h"
#include "ModelUtilities.h"
using namespace ModelUtilities;
ModelController::ModelController() {
	focusedModel = NULL;
	models = vector<Model*>();
	selectedModelIndex = -1;
}

ModelController::~ModelController() {
	focusedModel = NULL;
}

void ModelController::addModel(Model* model) {
	models.push_back(model);
}

void ModelController::initModels(int shaderProgram, unsigned int vao, unsigned int vbo, Sphere sphere) {
	Model* axisModel = new Model(vec3(0.0f, 0.0f, 0.0f), 0.0f); //axis lines
	TaqiModel* taqi = new TaqiModel(vec3(-40.0f, 0.0f, -40.0f), 0.0f); //Taqi (Q4)
	HauModel* hau = new HauModel(vec3(40.0f, 0.0f, -40.0f), 0.0f); //Hau (U6)
	RoyModel* roy = new RoyModel(vec3(-40.0f, 0.0f, 40.0f), 0.0f); //Roy (Y8)
	SwetangModel* swetang = new SwetangModel(vec3(0.0f, 0.0f, 0.0f), 0.0f); //Swetang (E0)
	WilliamModel* william = new WilliamModel(vec3(40.0f, 0.0f, 40.0f), 0.0f); //William (L9)

	models.push_back(axisModel);
	models.push_back(taqi);
	models.push_back(hau);
	models.push_back(roy);
	models.push_back(swetang);
	models.push_back(william);

	setModelsShaderProgram(shaderProgram);
	setModelsVAO(vao);
	setModelsVBO(vbo);
	setModelsSphere(sphere);
	modelFocusSwitch(4);
}

void ModelController::drawModels(mat4 worldRotationUpdate, GLuint textureArray[], int shaderProgram) {
	for (vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setShaderProgram(shaderProgram);
		(*model)->draw(worldRotationUpdate, textureArray);
	}
}

/// <summary>
/// Swaps model being controlled by user
/// </summary>
/// <param name="nextModel">Index of selected model to focus movement on</param>
void ModelController::modelFocusSwitch(int nextModel) {
	//Don't update anything if model to switch to is current model
	if (selectedModelIndex == nextModel)
		return;

	focusedModel = models[nextModel];
}

void ModelController::setModelsShaderProgram(int shaderProgram) {
	for(vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setShaderProgram(shaderProgram);
	}
}

void ModelController::setModelsVAO(unsigned int vao) {
	for (vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setVao(vao);
	}
}

void ModelController::setModelsVBO(unsigned int vbo) {
	for (vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setVbo(vbo);
	}
}

void ModelController::setModelsSphere(Sphere sphere) {
	for (vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setSphere(sphere);
	}
}

void ModelController::randomPosition(vec3 value) {
	focusedModel->setPosition(value); 
}