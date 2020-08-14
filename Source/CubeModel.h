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

enum RubiksMove {
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
	bool getIsTurning() { return isTurning; }
	float getRotationAngle() { return rotationAngle; }
	float getDt() { return dt; }
	vec3 getRotationVector() { return rotationVector; }
	RubiksMove getCurrentMove() { return currentMove; }
	vec3 getNextPosition() { return nextPosition; }
	vec3 getRubiksCubePostion() { return rubiksCubePosition; }

	void setType(CubeType type) { this->type = type; }
	void setId(int id) { this->id = id; }
	void setRelativePositions(vector<CubePosition> relativePositions) { this->relativePositions = relativePositions; }
	void setIsTurning(bool isTurning) { this->isTurning = isTurning; }
	void setRotationAngle(float rotationAngle) { this->rotationAngle = rotationAngle; }
	void setDt(float dt) { this->dt = dt; }
	void setRotationVector(vec3 rotationVector) { this->rotationVector = rotationVector; }
	void setCurrentMove(RubiksMove currentMove) { this->currentMove = currentMove; }
	void setCurrentAction(RubiksMove currentMove, vec3 rotationVector);
	void setNextPosition(vec3 nextPosition) { this->nextPosition = nextPosition; }
	void setRubiksCubePosition(vec3 rubiksCubePosition) { this->rubiksCubePosition = rubiksCubePosition; }

	mat4 computeSingleCubeRotation();
	mat4 computeRotationMatrix();
	bool equal(CubeModel cube);

	//void updateActionRotationMatrix();
	virtual void draw(mat4 worldRotationUpdate, GLuint toggle, GLuint texture);

private:
	int id;
	CubeType type;
	bool isTurning;
	float rotationAngle;
	float baseRotationAngle;
	vec3 rotationVector;
	vector<CubePosition> relativePositions; 
	float dt;
	RubiksMove currentMove;
	vector<RubiksMove> moveList;
	vector<float> rotationAngleList;
	vector<vec3> rotationVectorList;
	vec3 nextPosition;
	vec3 rubiksCubePosition;

	void completeAction();
};