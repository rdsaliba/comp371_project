#include "CubeModel.h"
CubeModel::CubeModel() : Model() {
	id = -1;
	isTurning = false;
	rotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	moveList = vector<RubiksMove>();
	rotationAngleList = vector<float>();
	rotationVectorList = vector<vec3>();
	currentMove = RubiksMove::NONE;
	baseRotationAngle = 0.0f;
	rubiksCubePosition = vec3(0.0f, 0.0f, 0.0f);
}

CubeModel::CubeModel(vec3 position, float size) :Model(position, size) {
	id = -1;
	isTurning = false;
	rotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	moveList = vector<RubiksMove>();
	rotationAngleList = vector<float>();
	rotationVectorList = vector<vec3>();
	currentMove = RubiksMove::NONE;
	baseRotationAngle = 0.0f;
	rubiksCubePosition = vec3(0.0f, 0.0f, 0.0f);
};

CubeModel::CubeModel(CubeModel* cube) {
	isTurning = false;
	rotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	id = cube->getId();
	type = cube->getType();
	relativePositions = cube->getRelativePositions();
	rubiksCubePosition = cube->getRubiksCubePostion();
}

CubeModel::~CubeModel() {}

void CubeModel::setCurrentAction(RubiksMove currentMove, vec3 rotationVector) {
	this->currentMove = currentMove;
	this->rotationVector = rotationVector;
	this->rotationAngle = 0.0f;
}

mat4 CubeModel::computeSingleCubeRotation() {
	mat4 rm;
	if (moveList.size() > 0) {
		vector<RubiksMove>::iterator aMove = moveList.begin();
		vector<vec3>::iterator rotationV = rotationVectorList.begin();
		vector<float>::iterator angle = rotationAngleList.begin();

		rm = rotate(mat4(1.0f), radians((*angle)), (*rotationV));
		for (++aMove, ++rotationV, ++angle; aMove != moveList.end(); ++aMove, ++rotationV, ++angle) {
			rm = rotate(mat4(1.0f), radians((*angle)), (*rotationV)) * rm;
		}
		return rm;
	}else
		return rotate(mat4(1.0f), radians(this->rotation.y), vec3(0.0f, 1.0f, 0.0f));
}

mat4 CubeModel::computeRotationMatrix() {
	float rotationSpeed = 20.0f;
	if (isTurning) {
		switch (currentMove) {
		case RubiksMove::R:
		case RubiksMove::L_PRIME:
		case RubiksMove::U:
		case RubiksMove::D_PRIME:
		case RubiksMove::F:
		case RubiksMove::B_PRIME:
		case RubiksMove::MV:
		case RubiksMove::MVS:
		case RubiksMove::MH_PRIME:
			rotationAngle -= (rotationSpeed * dt);
			break;
		case RubiksMove::L:
		case RubiksMove::R_PRIME:
		case RubiksMove::U_PRIME:
		case RubiksMove::D:
		case RubiksMove::F_PRIME:
		case RubiksMove::B:
		case RubiksMove::MV_PRIME:
		case RubiksMove::MVS_PRIME:
		case RubiksMove::MH:
			rotationAngle += (rotationSpeed * dt);
			break;
		}
	}

	return rotate(mat4(1.0f), radians(rotationAngle), rotationVector);
}


/// <summary>
/// Evaluates equality of 2 cubes based on attributes relevant to the rubiks cube
/// </summary>
/// <param name="cube">cube to compare to 'this'</param>
/// <returns>true if they should be considered equal</returns>
bool CubeModel::equal(CubeModel cube) {
	if (this->id != cube.getId())
		return false;
	if (this->type != cube.getType())
		return false;
	if (this->relativePositions.size() != cube.getRelativePositions().size())
		return false;

	//Compare their set of relative positions within the rubiks cube
	bool hasPosition = false;
	vector<CubePosition>::iterator otherCubeEnd = cube.getRelativePositions().end();
	for (vector<CubePosition>::iterator thisPosition = relativePositions.begin(); thisPosition != relativePositions.end(); ++thisPosition) {
		//compare each element to each element of compared cube in case they have been inserted in a different order
		for (vector<CubePosition>::iterator otherPosition = cube.getRelativePositions().begin(); otherPosition != otherCubeEnd; ++otherPosition) {
			if ((*thisPosition) == (*otherPosition))
				hasPosition = true;
		}
		if (!hasPosition)
			return false;
	}

	return true;
}

void CubeModel::completeAction() {
	float alignmentOffset;
	if (rotationAngle > 0) {
		alignmentOffset = 90.0f - rotationAngle;
	}
	else {
		alignmentOffset = abs(rotationAngle) - 90.0f;
	}
	rotationAngle += alignmentOffset;

	isTurning = false;
	position = nextPosition;
	floor(rotationAngle);

	rotationAngleList.push_back(this->rotationAngle);
	rotationVectorList.push_back(this->rotationVector);
	moveList.push_back(this->currentMove);
	this->rotationAngle = 0.0f;
}

void CubeModel::draw(mat4 worldRotationUpdate, GLuint textureArray) {
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindTexture(GL_TEXTURE_2D, textureArray);

	GLuint worldMatrixLocation = glGetUniformLocation(this->shaderProgram, "worldMatrix");

	mat4 rotationUpdate = computeSingleCubeRotation();

	mat4 scaleUpdate = scale(mat4(1.0f), vec3(1.0f + this->scaling));
	mat4 groupMatrix = translate(mat4(1.0f), this->position) * rotationUpdate * scaleUpdate;
	groupMatrix = isTurning ? computeRotationMatrix() * groupMatrix : groupMatrix;
	groupMatrix = translate(mat4(1.0f), rubiksCubePosition) * groupMatrix;
	if (abs(rotationAngle) > 90.0f){
		completeAction();
	}
	
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groupMatrix[0][0]);
	glDrawArrays(this->renderMode, 0, 36);
}