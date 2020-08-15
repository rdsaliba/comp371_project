#include "ModelController.h"
#include "ModelUtilities.h"
#include "RubiksModel.h"
using namespace ModelUtilities;
ModelController::ModelController() {
	focusedModel = NULL;
	models = vector<Model*>();
	selectedModelIndex = -1;
	isScrambling = false;
	isAutoSolving = false;
}

ModelController::~ModelController() {
	focusedModel = NULL;
	rubiksCube = NULL;
	for (int i = 0; i < models.size(); i++)
		delete models[i];
}

/// <summary>
/// Adds Models to the list of models handled
/// </summary>
/// <param name="model"></param>
void ModelController::addModel(Model* model) {
	models.push_back(model);
}

/// <summary>
/// Initializes the models with their initial positions along with other shared attributes
/// </summary>
/// <param name="shaderProgram">Shader to assign to models</param>
/// <param name="vao"></param>
/// <param name="vbo"></param>
/// <param name="sphere">Sphere floating on each letter/digit models</param>
void ModelController::initModels(int shaderProgram, unsigned int vao, unsigned int vbo, Sphere sphere) {
	RubiksModel* rubiksModel = new RubiksModel(vec3(0.0f, 5.0f, 0.0f), 0.0f); //axis lines
	TaqiModel* taqi = new TaqiModel(vec3(-40.0f, 0.0f, -40.0f), 0.0f); //Taqi (Q4)
	HauModel* hau = new HauModel(vec3(40.0f, 0.0f, -40.0f), 0.0f); //Hau (U6)
	RoyModel* roy = new RoyModel(vec3(-40.0f, 0.0f, 40.0f), 0.0f); //Roy (Y8)
	SwetangModel* swetang = new SwetangModel(vec3(0.0f, 0.0f, -15.0f), 0.0f); //Swetang (E0)
	WilliamModel* william = new WilliamModel(vec3(40.0f, 0.0f, 40.0f), 0.0f); //William (L9)

	rubiksCube = rubiksModel;

	models.push_back(rubiksModel);
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
	rubiksModel->buildCubes();
}

/// <summary>
/// Draws/render all models handled by the controller
/// </summary>
/// <param name="worldRotationUpdate">change in scene rotation value</param>
/// <param name="textureArray">The textures to be applied on the models</param>
/// <param name="shaderProgram">Shader to use to render the models</param>
void ModelController::drawModels(mat4 worldRotationUpdate, GLuint textureArray[], int shaderProgram) {
	rubiksCube->setDt(dt);
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

/// <summary>
/// Assigns a shader to to all handled models
/// </summary>
/// <param name="shaderProgram"></param>
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

/// <summary>
/// Sets the mesh sphere object onto the models. 
/// </summary>
/// <param name="sphere"></param>
void ModelController::setModelsSphere(Sphere sphere) {
	for (vector<Model*>::iterator model = models.begin(); model != models.end(); ++model)
	{
		(*model)->setSphere(sphere);
	}
}

/// <summary>
/// Sets the Position of the focused model to a random location on the ground grid
/// </summary>
/// <param name="value"></param>
void ModelController::randomPosition(vec3 value) {
	focusedModel->setPosition(value); 
}

/// <summary>
/// Generates a random list of moves to scramble the rubiks cube
/// </summary>
void ModelController::scrambleGenerator() {
	srand(glfwGetTime()); //generate new seed to ensure scrambles are different
	int moveCtr = rand() % 10 + 3; //10 maximum moves, 3 moves minimum
	RubiksMove move = RubiksMove::NONE;
	isScrambling = true;
	for (int i = 0; i < moveCtr; i++) {
		do {
			move = static_cast<RubiksMove>(rand() % 20);
		} while (move == RubiksMove::NONE || move == RubiksMove::COMPLETE); //Prevents adding to scramble non rotational moves
	
		scrambleList.push_back(move);
	}
}

/// <summary>
/// Executes actions on the rubiks cube that do not require user input.
/// </summary>
void ModelController::automatedCubeAction() {
	scramble();
	solve();
}

/// <summary>
/// Applies the scramble moves onto the rubiks cube
/// </summary>
void ModelController::scramble() {
	if (isScrambling) {
		if (!scrambleList.empty()){
			if (!rubiksCube->getIsTurning()) {
				RubiksMove move = scrambleList.front();
				useRubiksCube(scrambleList.front());
				scrambleList.erase(scrambleList.begin());
			}
		}
		else {
			isScrambling = false;
		}
	}
}

/// <summary>
/// Auto solve the Rubiks cube
/// </summary>
void ModelController::solve() {
	if (isAutoSolving) {
		if (!solveList.empty()) {
			if (!rubiksCube->getIsTurning()) {
				useRubiksCube(solveList.back());
				solveList.pop_back();
			}
		}
		else {
			isAutoSolving = false;
		}
	}
}

/// <summary>
/// Executes a move on a layer of the rubiks cube.
/// Note: In this case a "move" refers to a rotation used to solve the rubiks cube rather than a manipulation of the 3D model in the scene.
/// </summary>
/// <param name="move">Move to apply onto the rubiks cube</param>
void ModelController::useRubiksCube(RubiksMove move) {
	if (!rubiksCube->getIsTurning()) {
		rubiksCube->queueMove(move);
		if(!isAutoSolving)
			solveList.push_back(getReverseMove(move));
		rubiksCube->updateActionState();
	
		switch (move) {
		case RubiksMove::L:
			rubiksCube->L();
			break;
		case RubiksMove::L_PRIME:
			rubiksCube->LPrime();
			break;
		case RubiksMove::U:
			rubiksCube->U();
			break;
		case RubiksMove::U_PRIME:
			rubiksCube->UPrime();
			break;
		case RubiksMove::D:
			rubiksCube->D();
			break;
		case RubiksMove::D_PRIME:
			rubiksCube->DPrime();
			break;
		case RubiksMove::F:
			rubiksCube->F();
			break;
		case RubiksMove::F_PRIME:
			rubiksCube->FPrime();
			break;
		case RubiksMove::B:
			rubiksCube->B();
			break;
		case RubiksMove::B_PRIME:
			rubiksCube->BPrime();
			break;
		case RubiksMove::R:
			rubiksCube->R();
			break;
		case RubiksMove::R_PRIME:
			rubiksCube->RPrime();
			break;
		case RubiksMove::MV:
			rubiksCube->MV();
			break;
		case RubiksMove::MV_PRIME:
			rubiksCube->MVPrime();
			break;
		case RubiksMove::MVS:
			rubiksCube->MVS();
			break;
		case RubiksMove::MVS_PRIME:
			rubiksCube->MVSPrime();
			break;
		case RubiksMove::MH:
			rubiksCube->MH();
			break;
		case RubiksMove::MH_PRIME:
			rubiksCube->MHPrime();
			break;
		}
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="move">Move applied on the rubiks cube</param>
/// <returns>Move negating the action previously applied on the cube</returns>
RubiksMove ModelController::getReverseMove(RubiksMove move) {
	RubiksMove reverseMove = RubiksMove::NONE;
	switch (move) {
	case RubiksMove::L:
		reverseMove = RubiksMove::L_PRIME;
		break;
	case RubiksMove::L_PRIME:
		reverseMove = RubiksMove::L;
		break;
	case RubiksMove::U:
		reverseMove = RubiksMove::U_PRIME;
		break;
	case RubiksMove::U_PRIME:
		reverseMove = RubiksMove::U;
		break;
	case RubiksMove::D:
		reverseMove = RubiksMove::D_PRIME;
		break;
	case RubiksMove::D_PRIME:
		reverseMove = RubiksMove::D;
		break;
	case RubiksMove::F:
		reverseMove = RubiksMove::F_PRIME;
		break;
	case RubiksMove::F_PRIME:
		reverseMove = RubiksMove::F;
		break;
	case RubiksMove::B:
		reverseMove = RubiksMove::B_PRIME;
		break;
	case RubiksMove::B_PRIME:
		reverseMove = RubiksMove::B;
		break;
	case RubiksMove::R:
		reverseMove = RubiksMove::R_PRIME;
		break;
	case RubiksMove::R_PRIME:
		reverseMove = RubiksMove::R;
		break;
	case RubiksMove::MV:
		reverseMove = RubiksMove::MV_PRIME;
		break;
	case RubiksMove::MV_PRIME:
		reverseMove = RubiksMove::MV;
		break;
	case RubiksMove::MVS:
		reverseMove = RubiksMove::MVS_PRIME;
		break;
	case RubiksMove::MVS_PRIME:
		reverseMove = RubiksMove::MVS;
		break;
	case RubiksMove::MH:
		reverseMove = RubiksMove::MH_PRIME;
		break;
	case RubiksMove::MH_PRIME:
		reverseMove = RubiksMove::MH;
		break;
	}

	return reverseMove;
}