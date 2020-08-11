#pragma once
#include "Model.h"
#include "ModelUtilities.h"
#include <cmath>
enum CubeType {
	CORNER,
	EDGE,
	CENTER,
	CORE
};

//Relative positions within a rubiks cube (i.e.: the faces)
enum CubePosition{
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	FRONT,
	BACK,
	MID_V,
	MID_H,
	MID_VS
};

enum class RubiksMove {
	L,
	L_PRIME,
	U,
	U_PRIME,
	D,
	D_PRIME,
	F,
	F_PRIME,
	B,
	B_PRIME,
	R,
	R_PRIME,
	NONE,
	COMPLETE,
	MH,
	MH_PRIME,
	MV,
	MV_PRIME,
	MVS,
	MVS_PRIME
};

class CubeModel : public Model {
public:
	CubeModel();
	CubeModel(vec3 position, float size);
	CubeModel(CubeModel* cube);
	virtual ~CubeModel();

	CubeType getType() { return type; }
	int getId() { return id; }
	vector<CubePosition> getRelativePositions() { return relativePositions; }
	mat4 getRotationMatrix() { return rotationMatrix; }
	mat4 getActionRotationMatrix() { return actionRotationMatrix; }
	bool getIsTurning() { return isTurning; }
	float getRotationAngle() { return rotationAngle; }
	float getDt() { return dt; }
	vec3 getRotationVector() { return rotationVector; }
	RubiksMove getCurrentMove() { return currentMove; }
	vec3 getNextPosition() { return nextPosition; }

	void setType(CubeType type) { this->type = type; }
	void setId(int id) { this->id = id; }
	void setRelativePositions(vector<CubePosition> relativePositions) { this->relativePositions = relativePositions; }
	void setRotationMatrix(mat4 rotationMatrix) { this->rotationMatrix = rotationMatrix; }
	void setActionRotationMatrix(mat4 actionRotationMatrix) { this->actionRotationMatrix = actionRotationMatrix; }
	void setIsTurning(bool isTurning) { this->isTurning = isTurning; }
	void setRotationAngle(float rotationAngle) { this->rotationAngle = rotationAngle; }
	void setDt(float dt) { this->dt = dt; }
	void setRotationVector(vec3 rotationVector) { this->rotationVector = rotationVector; }
	void setCurrentMove(RubiksMove currentMove) { this->currentMove = currentMove; }
	void setCurrentAction(RubiksMove currentMove, vec3 rotationVector);
	void setNextPosition(vec3 nextPosition) { this->nextPosition = nextPosition; }

	mat4 computeSingleCubeRotation();
	//mat4 computeRotationMatrices();
	mat4 computeRotationMatrix();
	bool equal(CubeModel cube);
	//void updateActionRotationMatrix();
	virtual void draw(mat4 worldRotationUpdate, GLuint textureArray);
private:
	CubeType type;
	mat4 rotationMatrix;
	mat4 actionRotationMatrix;
	bool isTurning;
	float rotationAngle;
	float currentMoveRotationAngle;
	float baseRotationAngle;
	vec3 rv;
	vec3 rotationVector;
	vector<CubePosition> relativePositions; 
	int id;
	float dt;
	RubiksMove currentMove;
	vector<RubiksMove> moveList;
	vector<float> rotationAngleList;
	vector<vec3> rotationVectorList;
	vec3 nextPosition;
};