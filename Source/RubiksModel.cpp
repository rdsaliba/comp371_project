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
	for (int i = 0; i < cubes.size(); i++)
		delete cubes[i];
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

	return getOrderedFaceCubes(faceCubes);
}

vector<CubeModel*> RubiksModel::getOrderedFaceCubes(vector<CubeModel*> faceCubes) {
	vector<CubeModel*> orderedFaceCubes;
	bool hasLeft = false, hasBack = false;
	int index = computeLowestFaceIndex(faceCubes, -1);
	while (index != cubes.size()) {
		for (vector<CubeModel*>::iterator cube = faceCubes.begin(); cube != faceCubes.end(); ++cube) {
			if ((*cube)->getId() == index) {
				orderedFaceCubes.push_back(*cube);
				faceCubes.erase(cube);
				break;
			}
		}
		index = computeLowestFaceIndex(faceCubes, index);
	}
	return orderedFaceCubes;
}


int RubiksModel::computeLowestFaceIndex(vector<CubeModel*> faceCubes, int lowestIndex) {
	int currentLowestIndex = cubes.size();
	int anId;
	for (vector<CubeModel*>::reverse_iterator cube = faceCubes.rbegin(); cube != faceCubes.rend(); ++cube) {
		anId = (*cube)->getId();
		if (anId < currentLowestIndex && anId > lowestIndex)
			currentLowestIndex = anId;
	}
	return currentLowestIndex;
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
				aCube->setRubiksCubePosition(this->position);
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
	case 0:
		positions.push_back(CubePosition::MID_V);
	}
	
	switch (y) {
	case -1:
		positions.push_back(CubePosition::BOTTOM);
		break;
	case 1:
		positions.push_back(CubePosition::TOP);
		break;
	case 0:
		positions.push_back(CubePosition::MID_H);
	}

	switch (z) {
	case -1:
		positions.push_back(CubePosition::BACK);
		break;
	case 1:
		positions.push_back(CubePosition::FRONT);
		break;
	case 0:
		positions.push_back(CubePosition::MID_VS);
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

/// <summary>
/// 
/// </summary>
/// <returns>The next move to apply onto the puzzle</returns>
RubiksMove RubiksModel::nextMove(){
	return moveBuffer.front();
}

/// <summary>
/// Puts the puzzle in an active state where an action is executed on a particular layer of the rubiks cube
/// </summary>
void RubiksModel::updateActionState() {
	isTurning = true;
	currentMove = moveBuffer.front();
	moveBuffer.erase(moveBuffer.begin());
}

/// <summary>
/// Returns the puzzle into an idle state once an action has been completed
/// </summary>
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
	vec3 rotationVector = computeRotationVector();

	//Creates a temporary copy of the cubes data before the rotation is applied
	for (int i = 0; i < size; i++) {
		cube = oldCubes[i];
		temp.setId(cube->getId());
		temp.setPosition(cube->getPosition());
		temp.setRelativePositions(cube->getRelativePositions());
		tempOldCubes.push_back(temp);
	}

	//Creates an ordered list of the cubes to be altered by the rotation
	for (int i = 2; i >= 0; i--) {
		for (int j = i; j < size; j += 3) {
			newCubes.push_back(workingCubes[j]);
		}
	}

	//Assign to the "rotated" cubes the data of the original cubes to complete the rotation on the
	//data side of the action.
	for (int i = 0; i < size; i++) {
		temp = tempOldCubes[i];
		cube = newCubes[i];
		cube->setId(temp.getId());
		cube->setRelativePositions(temp.getRelativePositions());
		cube->setNextPosition(temp.getPosition());
		cube->setIsTurning(true);
		cube->setCurrentAction(currentMove, rotationVector);
	}
}

/// <summary>
/// Determine the axis on which the rotation of an action will occur
/// </summary>
/// <returns>vec3 representing the rotation plane</returns>
vec3 RubiksModel::computeRotationVector() {
	vec3 rotationVector(0.0f);
	switch (currentMove) {
	case RubiksMove::R_PRIME:
	case RubiksMove::L:
	case RubiksMove::L_PRIME:
	case RubiksMove::R:
	case RubiksMove::MV:
	case RubiksMove::MV_PRIME:
		rotationVector.x = 1.0f;
		break;
	case RubiksMove::U:
	case RubiksMove::D_PRIME:
	case RubiksMove::U_PRIME:
	case RubiksMove::D:
	case RubiksMove::MH:
	case RubiksMove::MH_PRIME:
		rotationVector.y = 1.0f;
		break;
	case RubiksMove::F:
	case RubiksMove::B_PRIME:
	case RubiksMove::F_PRIME:
	case RubiksMove::B:
	case RubiksMove::MVS:
	case RubiksMove::MVS_PRIME:
		rotationVector.z = 1.0f ;
		break;

	}
	return rotationVector;
}

/// <summary>
/// Performs a clockwise rotation of the left side of the rubiks cube
/// </summary>
void RubiksModel::L() {
	vector<CubeModel*> oldCubes = getFaceCubes(CubePosition::LEFT);
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

/// <summary>
/// Performs a clockwise rotation of the top side of the rubiks cube
/// </summary>
void RubiksModel::U() {
	vector<CubeModel*> uCubes = getFaceCubes(CubePosition::TOP);
	updatRotatedLayerCubes(uCubes, uCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the top side of the rubiks cube
/// </summary>
void RubiksModel::UPrime() {
	vector<CubeModel*> uCubes = getFaceCubes(CubePosition::TOP);
	vector<CubeModel*> uReverseCubes = reverseCubeModelVector(uCubes);

	updatRotatedLayerCubes(uCubes, uReverseCubes);
}

/// <summary>
/// Performs a clockwise rotation of the bottom side of the rubiks cube
/// </summary>
void RubiksModel::D() {
	vector<CubeModel*> dCubes = getFaceCubes(CubePosition::BOTTOM);
	vector<CubeModel*> dReverseCubes = reverseCubeModelVector(dCubes);

	updatRotatedLayerCubes(dCubes, dReverseCubes);
}


/// <summary>
/// Performs an anticlockwise rotation of the bottom side of the rubiks cube
/// </summary>
void RubiksModel::DPrime() {
	vector<CubeModel*> dCubes = getFaceCubes(CubePosition::BOTTOM);
	updatRotatedLayerCubes(dCubes, dCubes);
}

/// <summary>
/// Performs a clockwise rotation of the front side of the rubiks cube
/// </summary>
void RubiksModel::F() {
	vector<CubeModel*> fCubes = getFaceCubes(CubePosition::FRONT);
	vector<CubeModel*> fReverseCubes = reverseCubeModelVector(fCubes);

	updatRotatedLayerCubes(fCubes, fReverseCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the front side of the rubiks cube
/// </summary>
void RubiksModel::FPrime() {
	vector<CubeModel*> fCubes = getFaceCubes(CubePosition::FRONT);
	updatRotatedLayerCubes(fCubes, fCubes);
}

/// <summary>
/// Performs a clockwise rotation of the back side of the rubiks cube
/// </summary>
void RubiksModel::B() {
	vector<CubeModel*> bCubes = getFaceCubes(CubePosition::BACK);
	updatRotatedLayerCubes(bCubes, bCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the back side of the rubiks cube
/// </summary>
void RubiksModel::BPrime() {
	vector<CubeModel*> bCubes = getFaceCubes(CubePosition::BACK);
	vector<CubeModel*> bReverseCubes = reverseCubeModelVector(bCubes);

	updatRotatedLayerCubes(bCubes, bReverseCubes);
}

/// <summary>
/// Performs a clockwise rotation of the right side of the rubiks cube
/// </summary>
void RubiksModel::R() {
	vector<CubeModel*> rCubes = getFaceCubes(CubePosition::RIGHT);
	vector<CubeModel*> rReverseCubes = reverseCubeModelVector(rCubes);

	updatRotatedLayerCubes(rCubes, rReverseCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the right side of the rubiks cube
/// </summary>
void RubiksModel::RPrime() {
	vector<CubeModel*> rCubes = getFaceCubes(CubePosition::RIGHT);
	updatRotatedLayerCubes(rCubes, rCubes);
}

/// <summary>
/// Performs a clockwise rotation of the middle-vertical layer of the rubiks cube
/// </summary>
void RubiksModel::MV() {
	vector<CubeModel*> mvCubes = getFaceCubes(CubePosition::MID_V);
	vector<CubeModel*> mvReverseCubes = reverseCubeModelVector(mvCubes);

	updatRotatedLayerCubes(mvCubes, mvReverseCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the middle-vertical layer of the rubiks cube
/// </summary>
void RubiksModel::MVPrime() {
	vector<CubeModel*> mvCubes = getFaceCubes(CubePosition::MID_V);
	updatRotatedLayerCubes(mvCubes, mvCubes);
}

/// <summary>
/// Performs a clockwise rotation of the middle-vertical-sideway layer of the rubiks cube
/// </summary>
void RubiksModel::MVS() {
	vector<CubeModel*> mvsCubes = getFaceCubes(CubePosition::MID_VS);
	vector<CubeModel*> mvsReverseCubes = reverseCubeModelVector(mvsCubes);

	updatRotatedLayerCubes(mvsCubes, mvsReverseCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the middle-vertical-sideway layer of the rubiks cube
/// </summary>
void RubiksModel::MVSPrime() {
	vector<CubeModel*> mvsCubes = getFaceCubes(CubePosition::MID_VS);
	updatRotatedLayerCubes(mvsCubes, mvsCubes);
}

/// <summary>
/// Performs a clockwise rotation of the middle-horizontal layer of the rubiks cube
/// </summary>
void RubiksModel::MH() {
	vector<CubeModel*> mhCubes = getFaceCubes(CubePosition::MID_H);
	vector<CubeModel*> mhReverseCubes = reverseCubeModelVector(mhCubes);

	updatRotatedLayerCubes(mhCubes, mhReverseCubes);
}

/// <summary>
/// Performs an anticlockwise rotation of the middle-horizontal layer of the rubiks cube
/// </summary>
void RubiksModel::MHPrime() {
	vector<CubeModel*> mhCubes = getFaceCubes(CubePosition::MID_H);
	updatRotatedLayerCubes(mhCubes, mhCubes);
}
/// <summary>
/// Creates a vector of CubeModel* that is the reverse order of a given vector
/// </summary>
/// <param name="cubeVector">Vector of CubeModel* ordered from last to first</param>
/// <returns></returns>
vector<CubeModel*> RubiksModel::reverseCubeModelVector(vector<CubeModel*> cubeVector) {
	vector<CubeModel*> reversedCubeVector;

	for (vector<CubeModel*>::reverse_iterator cube = cubeVector.rbegin(); cube != cubeVector.rend(); ++cube) {
		reversedCubeVector.push_back(*cube);
	}

	return reversedCubeVector;
}

/// <summary>
/// Draws the rubiks cube in the 3D scene
/// </summary>
/// <param name="worldRotationUpdate">Change in scene's rotation since last rendering of the model</param>
/// <param name="textureArray">Textures to be applied onto the </param>
void RubiksModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {

	//mat4 rotationMatrix(0.0f);
	//if (isTurning) {
	//	mat4 rotationMatrix = this->computeRotationMatrix();
	//}
	int i = 4;

	bool turning = false;

	for (vector<CubeModel*>::iterator aCube = cubes.begin(); aCube != cubes.end(); ++aCube) {
		
		(*aCube)->setShaderProgram(this->getShaderProgram());
		(*aCube)->setDt(dt);
		(*aCube)->draw(worldRotationUpdate, textureArray[0], textureArray[i]);

		
		if ((*aCube)->getIsTurning()) {
			turning = true; //If there is at least 1 cube that is still turning, then the rubiks cube is still turning
		}
		i++;
	}

	if (!turning) {
		completeCurrentAction();
	}
}