#include "CubeModel.h"
CubeModel::CubeModel() : Model() {
	id = -1;
	isTurning = false;
	rotationAngle = 0.0f;
	currentMoveRotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	moveList = vector<RubiksMove>();
	rotationAngleList = vector<float>();
	rotationVectorList = vector<vec3>();
	currentMove = RubiksMove::NONE;
	rv = vec3(0.0f, 1.0f, 0.0f);
	baseRotationAngle = 0.0f;
	//actionRotationMatrix = rotate(mat4(1.0f), radians(rotationAngle), rotationVector);
}

CubeModel::CubeModel(vec3 position, float size) :Model(position, size) {
	id = -1;
	isTurning = false;
	rotationAngle = 0.0f;
	currentMoveRotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	moveList = vector<RubiksMove>();
	rotationAngleList = vector<float>();
	rotationVectorList = vector<vec3>();
	currentMove = RubiksMove::NONE;
	rv = vec3(0.0f, 1.0f, 0.0f);
	baseRotationAngle = 0.0f;
	//actionRotationMatrix = rotate(mat4(1.0f), radians(rotationAngle), rotationVector);
};

CubeModel::CubeModel(CubeModel* cube) {
	isTurning = false;
	rotationAngle = 0.0f;
	rotationVector = vec3(0.0f, 1.0f, 0.0f);
	id = cube->getId();
	type = cube->getType();
	relativePositions = cube->getRelativePositions();
}

CubeModel::~CubeModel() {
	/*delete rotationAngle;
	delete rotationVector;*/
	/*moveList.clear();
	rotationVectorList.clear();
	rotationAngleList.clear();*/

}

void CubeModel::setCurrentAction(RubiksMove currentMove, vec3 rotationVector) {
	//if (currentMove == RubiksMove::COMPLETE) {
	//	rotationAngleList.push_back(this->rotationAngle);
	//	rotationVectorList.push_back(this->rotationVector);
	//	moveList.push_back(this->currentMove);
	//	/*baseRotationAngle = this->rotationAngle;
	//	this->rv = this->rotationVector;*/
	//}
	//else if (this->currentMove == RubiksMove::NONE) {
	//	this->currentMove = currentMove;
	//	this->rotationVector = rotationVector;
	//	this->rotationAngle = 0.0f;
	//}
	this->currentMove = currentMove;
	this->rotationVector = rotationVector;
	this->rotationAngle = 0.0f;
	//	if (currentMove != this->currentMove) {
	//		baseRotationAngle = this->rotationAngle;
	//	}
	//	else {
	//		baseRotationAngle += this->rotationAngle;
	//	}
	//	this->rv = this->rotationVector;

	//}else {
	//	this->currentMove = currentMove;
	//	this->rotationVector = rotationVector;
	//	this->rotationAngle = 0.0f;
	//}
	//else {
		/*moveList.push_back(this->currentMove);
		rotationVectorList.push_back(this->rotationVector);
		rotationAngleList.push_back(this->rotationAngle);
		this->currentMove = currentMove;
		this->rotationVector = rotationVector;
		this->rotationAngle = 0.0f;*/
	//}
}
//
//void CubeModel::setRotationVector(vec3* rotationVector) {
//	if (rotationVectorList.size() > 1) {
//		rotationVectorList.push_back(this->rotationVector);
//	}
//	else {
//		rotationVectorList.push_back(rotationVector); //first move
//	}
//	this->rotationVector = rotationVector; 
//}

//void CubeModel::setCurrentMove(RubiksMove currentMove) {
//	if (moveList.size() > 1 ) {
//		moveList.push_back((this->currentMove));
//	}
//	else {
//		moveList.push_back(currentMove); //first move
//	}
//	this->currentMove = currentMove; 
//}
//
//void CubeModel::updateActionRotationMatrix() {
//	//vec3 rotationVector(0.0f);
//	switch (currentMove) {
//	case RubiksMove::R_PRIME:
//	case RubiksMove::L:
//		rotationAngle += -90.0f;
//		rotationVector.x = 1.0f;
//		break;
//	case RubiksMove::L_PRIME:
//	case RubiksMove::R:
//		//rotationAngle = 1.0f;
//		rotationAngle += 90.0f;
//		rotationVector.x = 1.0f;
//		break;
//	}
//
//	actionRotationMatrix = rotate(mat4(1.0f), radians(rotationAngle), rotationVector);
//}

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

//mat4 CubeModel::computeRotationMatrices() {
//	mat4 rotationMatrix(1.0f); 
//	if (moveList.size() > 0) {
//		vector<RubiksMove>::iterator aMove = moveList.begin();
//		vector<vec3>::iterator rotationV = rotationVectorList.begin();
//		vector<float>::iterator angle = rotationAngleList.begin();
//
//		for (; aMove != moveList.end(); ++aMove, ++rotationV, ++angle) {
//			rotationMatrix *= rotate(mat4(1.0f), radians((*angle)), (*rotationV));
//		}
//		return computeRotationMatrix() * rotationMatrix;
//	}
//	else {
//		return computeRotationMatrix();
//	}
//}

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
			//rotationAngle = -1.0f;
			rotationAngle -= (rotationSpeed * dt);
			currentMoveRotationAngle -= (rotationSpeed * dt);
			//rotationVector.x = 1.0f;
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
			//rotationAngle = 1.0f;
			rotationAngle += (rotationSpeed * dt);
			currentMoveRotationAngle += (rotationSpeed * dt);
			//rotationVector.x = 1.0f;
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

void CubeModel::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLuint worldMatrixLocation = glGetUniformLocation(this->shaderProgram, "worldMatrix");

	mat4 rotationUpdate = computeSingleCubeRotation();//rotate(mat4(1.0f), radians(this->baseRotationAngle), rv);
	//if (isTurning) {
	//	rotationUpdate = rotationMatrix/* rotationUpdate;
	//}
	mat4 scaleUpdate = scale(mat4(1.0f), vec3(1.0f + this->scaling));
	//	mat4 shearUpdate = shearY3D(mat4(1.0f), 0.0f, shearYZ.z);
	mat4 groupMatrix = translate(mat4(1.0f), this->position) * rotationUpdate * scaleUpdate; //* shearUpdate * scaleUpdate;
	//if (isTurning) {
		groupMatrix = isTurning ? computeRotationMatrix() * groupMatrix : groupMatrix;
		if (abs(currentMoveRotationAngle) > 90.0f){
			float alignmentOffset;
			if (rotationAngle > 0) {
				alignmentOffset = 90.0f - currentMoveRotationAngle;
			}
			else {
				alignmentOffset = abs(currentMoveRotationAngle) - 90.0f;
			}
			rotationAngle += alignmentOffset;

			isTurning = false;
			position = nextPosition;
			currentMoveRotationAngle = 0.0f;
			floor(rotationAngle);
			
			rotationAngleList.push_back(this->rotationAngle);
			rotationVectorList.push_back(this->rotationVector);
			moveList.push_back(this->currentMove);
			this->rotationAngle = 0.0f;
			//setCurrentAction(RubiksMove::COMPLETE, this->rotationVector);
			//updateActionRotationMatrix();
		}
	/*}
	else {
		groupMatrix = actionRotationMatrix * groupMatrix;
	}*/
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groupMatrix[0][0]);
	glDrawArrays(this->renderMode, 0, 36);
	//isTurning = false;
	//Model::draw(worldRotationUpdate, textureArray);
}