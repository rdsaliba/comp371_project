#pragma once
#include <vector>
#include "Model.h"
#include "ModelUtilities.h"
class RubiksModel : public Model {
public:
	RubiksModel();
	RubiksModel(vec3 position, float size);
	~RubiksModel();
	void buildCubes();

	vector<CubeModel*> getFaceCubes(CubePosition face);
	vector<CubeModel*> getOrderedFaceCubes(vector<CubeModel*> faceCubes);
	vector<CubeModel*> getCubesByType(CubeType cubeType, vector<CubeModel*> cubes);
	RubiksMove getCurrentMove() { return currentMove; }
	float getDt() { return dt; }
	bool getIsTurning() { return isTurning; }

	void setDt(float dt) { this->dt = dt; }
	void setIsTurning(bool isTurning) { this->isTurning = isTurning; }

	int computeLowestFaceIndex(vector<CubeModel*> faceCubes, int lowestIndex);
	void updateActionState();
	void completeCurrentAction();
	void queueMove(RubiksMove move) { moveBuffer.push_back(move); }

	//Cube manipulation/rotation functions, using rubiks cube notation
	void L();
	void LPrime();
	void U();
	void UPrime();
	void D();
	void DPrime();
	void F();
	void FPrime();
	void B();
	void BPrime();
	void R();
	void RPrime();

	vec3 computeRotationVector();
	bool computeSolveState();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray[]);
private:
	vector<CubeModel*> cubes;
	vector<CubeModel> solutionCubes;
	vector<int> centerIndices;
	vector<RubiksMove> moveBuffer;
	RubiksMove currentMove;
	vector<CubeModel*> activeLayer;
	vector<CubeModel> rotatedLayer;
	float currentActionAngle;
	float dt;
	bool isSolved;
	bool isTurning;

	float addComponentCubeOffset(float value);
	CubeModel* initCubeComponent(float x, float y, float z);
	vector<CubePosition> getComponentPosition(int x, int y, int z);
	void updatRotatedLayerCubes(vector<CubeModel*> oldCubes, vector<CubeModel*> workingCubes);
	//mat4 computeRotationMatrix();
	vector<CubeModel*> reverseCubeModelVector(vector<CubeModel*> cubeVector);
};