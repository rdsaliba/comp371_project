#include "RubiksModel.h"
#include <cmath>    
#include <algorithm>
RubiksModel::RubiksModel() {
	this->cubes = vector<CubeModel*>();
	this->centerIndices = vector<int>();
	this->isTurning = false;
	this->currentActionAngle = 0.0f;
	this->isSolved = false;
	this->dt = 0;
}

RubiksModel::RubiksModel(vec3 position, float size) :Model(position, size) {
	this->cubes = vector<CubeModel*>();
	this->centerIndices = vector<int>();
	this->isTurning = false;
	this->currentActionAngle = 0.0f;
	this->isSolved = false;
	this->dt = 0;
};

RubiksModel::~RubiksModel() {
	for (vector<CubeModel*>::iterator aCube = cubes.begin(); aCube != cubes.end(); ++aCube) {
		delete (*aCube);
	}
}

/// <summary>
/// Evaluates if the rubiks cube is solved. i.e: All cube components are in their expected location
/// </summary>
/// <returns></returns>
bool RubiksModel::computeSolveState() {
	for (vector<CubeModel*>::iterator cube = cubes.begin(); cube != cubes.end(); ++cube) {
		(*cube)->setSize(0.0f);
	}

	vector<CubeModel>::iterator solutionCube = solutionCubes.begin();
	vector<CubePosition> currentPositions, solutionPositions;
	for (vector<CubeModel*>::iterator cube = cubes.begin(); cube != cubes.end(); ++cube, ++solutionCube) {
		if ((*cube)->getId() != (*solutionCube).getId())
			return false;
		if ((*cube)->getType() != (*solutionCube).getType())
			return false;

		currentPositions = (*cube)->getRelativePositions();
		solutionPositions = (*solutionCube).getRelativePositions();
		if (currentPositions.size() != solutionPositions.size())
			return false;
		
		for (int i = 0; i < currentPositions.size(); i++) {
			if (currentPositions[i] != solutionPositions[i])
				return false;
		}
		/*if (!(*cube)->equal((*solutionCube)))
			return false;*/
	}

	for (vector<CubeModel*>::iterator cube = cubes.begin(); cube != cubes.end(); ++cube) {
		(*cube)->setSize(5.0f);
	}
	return true;
}

/// <summary>
/// Gets pointers of each cubes of a given face of a rubiks cube
/// </summary>
/// <param name="face">the face of the rubiks cube you want to fetch the components from</param>
/// <returns></returns>
vector<CubeModel*> RubiksModel::getFaceCubes(CubePosition face) {
	vector<CubeModel*> faceCubes;
	for (vector<CubeModel*>::iterator cube = cubes.begin(); cube != cubes.end(); ++cube) {
		vector<CubePosition> cubePositions = (*cube)->getRelativePositions();

		if (std::find(cubePositions.begin(), cubePositions.end(), face) != cubePositions.end()) {
			faceCubes.push_back(*cube);
		}
	}

	return faceCubes;
}

vector<CubeModel*> RubiksModel::getCubesByType(CubeType cubeType, vector<CubeModel*> cubes) {
	vector<CubeModel*> resultCubes;
	for (vector<CubeModel*>::iterator cube = cubes.begin(); cube != cubes.end(); ++cube) {
		if ((*cube)->getType() == cubeType) {
			resultCubes.push_back((*cube));
		}
	}

	return resultCubes;
}

/// <summary>
/// Generates the cube models making up each pieces of a rubiks cube
/// </summary>
void RubiksModel::buildCubes() {
	CubeModel* aCube = NULL;
	int index = 0, sliceIndex = 0;
	CubeType type;
	for (int x = -1; x < 2; x++, sliceIndex = 0) {
		for (int y = -1; y < 2; y++, sliceIndex++) {
			for (int z = -1; z < 2; z++, index++, sliceIndex++) {
				aCube = initCubeComponent((float)x, (float)y, (float)z);
				aCube->setRelativePositions(getComponentPosition(x, y, z));
				aCube->setId(index);
				//Find centers
				if ( (((x == y) || (x == z)) && (x == 0)) || (((y == x) || (y == z)) && (y == 0))) {
					type = CubeType::CENTER;
					centerIndices.push_back(index);
				}
				else {
					switch (index % 2) {
					case 0:
						type = CubeType::CORNER;
						break;
					case 1:
						type = CubeType::EDGE;
						break;
					}
					if (index == 13) {
						type = CubeType::CORE;
					}
				}
 			
				aCube->setType(type);
				cubes.push_back(aCube);
				CubeModel solutionCube(aCube);
				solutionCubes.push_back(solutionCube);
			}
		}
	}
}

/// <summary>
/// Computes the position of a cube given its xyz values
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns></returns>
vector<CubePosition> RubiksModel::getComponentPosition(int x, int y, int z) {
	vector<CubePosition> positions;
	switch (x) {
	case -1:
		positions.push_back(CubePosition::LEFT);
		break;
	case 1:
		positions.push_back(CubePosition::RIGHT);
		break;
	}
	
	switch (y) {
	case -1:
		positions.push_back(CubePosition::BOTTOM);
		break;
	case 1:
		positions.push_back(CubePosition::TOP);
		break;
	}

	switch (z) {
	case -1:
		positions.push_back(CubePosition::BACK);
		break;
	case 1:
		positions.push_back(CubePosition::FRONT);
		break;
	}
	
	return positions;
}


/// <summary>
/// Add an offset to component cube to have spacing between the cubes.
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
float RubiksModel::addComponentCubeOffset(float value) {
	if (value != 0)
		value += value < 0 ? -0.2f : 0.2f;
	return value;
}

/// <summary>
/// Build 1 cube of the rubiks cube
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns></returns>
CubeModel* RubiksModel::initCubeComponent(float x, float y, float z) {
	float xValue, yValue, zValue = 0.0f;
	xValue = addComponentCubeOffset(x);
	yValue = addComponentCubeOffset(y);
	zValue = addComponentCubeOffset(z);
	CubeModel* aCube = new CubeModel(vec3(xValue, yValue, zValue), 0.0f);
	aCube->setVao(this->vao);
	aCube->setVbo(this->vbo);

	return aCube;
}

void RubiksModel::updateActionState() {
	isTurning = true;
	currentMove = moveBuffer.front();
	moveBuffer.erase(moveBuffer.begin());
}

void RubiksModel::completeCurrentAction() {
	isTurning = false;
	currentActionAngle = 0.0f;
	isSolved = computeSolveState();
}

/// <summary>
/// Performs reassignment of a layer's cubes to update their attributes to their new positions
/// </summary>
/// <param name="oldCubes">The original positions/order of the cubes</param>
/// <param name="workingCubes">The list of cubes to apply reordering onto</param>
void RubiksModel::updatRotatedLayerCubes(vector<CubeModel*> oldCubes, vector<CubeModel*> workingCubes) {
	vector<CubeModel*> newCubes;
	int size = oldCubes.size();
	vector<CubeModel> tempOldCubes;
	CubeModel temp;
	CubeModel* cube = NULL;
	activeLayer = oldCubes;
	/*mat4 rotationMatrix = computeRotationMatrix();*/
	vec3 rotationVector = computeRotationVector();
	for (int i = 0; i < size; i++) {
		cube = oldCubes[i];
		temp.setPosition(cube->getPosition());
		temp.setRelativePositions(cube->getRelativePositions());
		tempOldCubes.push_back(temp);
	}
	

	for (int i = 2; i >= 0; i--) {
		for (int j = i; j < size; j += 3) {
			newCubes.push_back(workingCubes[j]);
		}
	}

	for (int i = 0; i < size; i++) {
		temp = tempOldCubes[i];
		cube = newCubes[i];
		/*cube->setRelativePositions(temp.getRelativePositions());
		cube->setIsTurning(true);*/
		cube->setRelativePositions(temp.getRelativePositions());
		cube->setNextPosition(temp.getPosition());
		//cube->setPosition(temp.getPosition());
		cube->setIsTurning(true);
		//cube->setActionRotationMatrix(rotationMatrix);
		//cube->setRotationAngle(currentActionAngle);
		/*cube->setRotationVector(rotationVector);
		cube->setCurrentMove(currentMove);*/
		cube->setCurrentAction(currentMove, rotationVector);
	}
}
vec3 RubiksModel::computeRotationVector() {
	vec3 rotationVector(0.0f);
	switch (currentMove) {
	case RubiksMove::R_PRIME:
	case RubiksMove::L:
		//currentActionAngle = -45.0f;
		//currentActionAngle -= (rotationSpeed * dt);
	case RubiksMove::L_PRIME:
	case RubiksMove::R:
		//currentActionAngle = 45.0f;
		//currentActionAngle += (rotationSpeed * dt);
		rotationVector.x = 1.0f;
		break;
	case RubiksMove::U:
	case RubiksMove::D_PRIME:
	case RubiksMove::U_PRIME:
	case RubiksMove::D:
		rotationVector.y = 1.0f;
		break;
	case RubiksMove::F:
	case RubiksMove::B_PRIME:
	case RubiksMove::F_PRIME:
	case RubiksMove::B:
		rotationVector.z = 1.0f;
		break;

	}
	return rotationVector;
}
//
//mat4 RubiksModel::computeRotationMatrix() {
//	vec3 rotationVector(0.0f);
//	float rotationSpeed = 0.5f;
//	switch (currentMove) {
//	case ModelUtilities::RubiksMove::R_PRIME:
//	case ModelUtilities::RubiksMove::L:
//		currentActionAngle = -1.0f;
//		//currentActionAngle -= (rotationSpeed * dt);
//		rotationVector.x = 1.0f;
//		break;
//	case ModelUtilities::RubiksMove::L_PRIME:
//	case ModelUtilities::RubiksMove::R:
//		currentActionAngle = 1.0f;
//		//currentActionAngle += (rotationSpeed * dt);
//		rotationVector.x = 1.0f;
//		break;
//	}
//
//	return rotate(mat4(1.0f), radians(currentActionAngle), rotationVector);
//}

/// <summary>
/// Performs a clockwise rotation of the left side of the rubiks cube
/// </summary>
void RubiksModel::L() {
	vector<CubeModel*> oldCubes = getFaceCubes(CubePosition::LEFT);
	activeLayer = oldCubes;
	updatRotatedLayerCubes(oldCubes, oldCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the left side of the rubiks cube
/// </summary>
void RubiksModel::LPrime() {
	vector<CubeModel*> lcubes = getFaceCubes(CubePosition::LEFT);
	vector<CubeModel*> lReverseCubes = reverseCubeModelVector(lcubes);

	updatRotatedLayerCubes(lcubes, lReverseCubes);
}

void RubiksModel::U() {
	vector<CubeModel*> uCubes = getFaceCubes(CubePosition::TOP);
	updatRotatedLayerCubes(uCubes, uCubes);
}

void RubiksModel::UPrime() {
	vector<CubeModel*> uCubes = getFaceCubes(CubePosition::TOP);
	vector<CubeModel*> uReverseCubes = reverseCubeModelVector(uCubes);

	updatRotatedLayerCubes(uCubes, uReverseCubes);
}

void RubiksModel::D() {
	vector<CubeModel*> dCubes = getFaceCubes(CubePosition::BOTTOM);
	updatRotatedLayerCubes(dCubes, dCubes);
}

void RubiksModel::DPrime() {
	vector<CubeModel*> dCubes = getFaceCubes(CubePosition::BOTTOM);
	vector<CubeModel*> dReverseCubes = reverseCubeModelVector(dCubes);

	updatRotatedLayerCubes(dCubes, dReverseCubes);
}

void RubiksModel::F() {
	vector<CubeModel*> fCubes = getFaceCubes(CubePosition::FRONT);
	updatRotatedLayerCubes(fCubes, fCubes);
}

void RubiksModel::FPrime() {
	vector<CubeModel*> fCubes = getFaceCubes(CubePosition::FRONT);
	vector<CubeModel*> fReverseCubes = reverseCubeModelVector(fCubes);

	updatRotatedLayerCubes(fCubes, fReverseCubes);
}

void RubiksModel::B() {
	vector<CubeModel*> bCubes = getFaceCubes(CubePosition::BACK);
	updatRotatedLayerCubes(bCubes, bCubes);
}

void RubiksModel::BPrime() {
	vector<CubeModel*> bCubes = getFaceCubes(CubePosition::BACK);
	vector<CubeModel*> bReverseCubes = reverseCubeModelVector(bCubes);

	updatRotatedLayerCubes(bCubes, bReverseCubes);
}

void RubiksModel::R() {
	vector<CubeModel*> rCubes = getFaceCubes(CubePosition::RIGHT);
	updatRotatedLayerCubes(rCubes, rCubes);
}

void RubiksModel::RPrime() {
	vector<CubeModel*> rCubes = getFaceCubes(CubePosition::RIGHT);
	vector<CubeModel*> rReverseCubes = reverseCubeModelVector(rCubes);

	updatRotatedLayerCubes(rCubes, rReverseCubes);
}

/// <summary>
	/// Creates a vector of CubeModel* that is the reverse order of a given vector
	/// </summary>
	/// <param name="cubeVector"></param>
	/// <returns></returns>
vector<CubeModel*> RubiksModel::reverseCubeModelVector(vector<CubeModel*> cubeVector) {
	vector<CubeModel*> reversedCubeVector;

	for (vector<CubeModel*>::reverse_iterator cube = cubeVector.rbegin(); cube != cubeVector.rend(); ++cube) {
		reversedCubeVector.push_back(*cube);
	}

	return reversedCubeVector;
}

void RubiksModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	//mat4 rotationMatrix(0.0f);
	//if (isTurning) {
	//	mat4 rotationMatrix = this->computeRotationMatrix();
	//}
	bool turning = false;
	for (vector<CubeModel*>::iterator aCube = cubes.begin(); aCube != cubes.end(); ++aCube) {
		(*aCube)->setShaderProgram(this->getShaderProgram());
	/*	if ((*aCube)->getIsTurning()) {
			(*aCube)->setActionRotationMatrix(rotationMatrix);
		}*/
		(*aCube)->setDt(dt);
		(*aCube)->draw(worldRotationUpdate, textureArray);
		if ((*aCube)->getIsTurning()) {
			turning = true;
		}
	}

	if (!turning) {
		completeCurrentAction();
	}
	/*if (abs(currentActionAngle) > 90.0f) {
		for (vector<CubeModel*>::iterator aCube = cubes.begin(); aCube != cubes.end(); ++aCube) {
			(*aCube)->isTurning = false;
			(*aCube)->setRotationMatrix(rotationMatrix);
		}
		completeCurrentAction();
	}*/
}