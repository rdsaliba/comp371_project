#include "Model.h"

Model::Model() {
	this->position = vec3(0.0f, 0.0f, 0.0f);
	this->scaling = 0.0f;
	this->rotation = vec3(1.0f, 1.0f, 1.0f);
	this->renderMode = GL_TRIANGLES;
	this->shaderProgram = -1;
	this->vao = -1;
	this->vbo = -1;
}

Model::Model(vec3 position, float scaling) {
	this->position = position;
	this->scaling = scaling;
	this->rotation = vec3(1.0f, 1.0f, 1.0f);
	this->renderMode = GL_TRIANGLES;
	this->shaderProgram = -1;
	this->vao = -1;
	this->vbo = -1;
}

Model::Model(const Model& model) {
	this->position = model.position;
	this->scaling = model.scaling;
	this->rotation = model.rotation;
	this->renderMode = model.renderMode;
	this->shaderProgram = model.shaderProgram;
	this->vao = model.vao;
	this->vbo = model.vbo;
}

Model::~Model() {
	/*glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);*/
}

void Model::updatePosition(vec3 moveVector) {
	this->position.x += moveVector.x;
	this->position.y += moveVector.y;
	this->position.z += moveVector.z;
}

void Model::draw(mat4 worldRotationUpdate, GLuint textureArray[]) {}

void Model::drawPart(mat4 worldRotationUpdate,mat4 part, vec3 componentPosition) {
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLuint worldMatrixLocation = glGetUniformLocation(this->shaderProgram, "worldMatrix");
	mat4 rotationUpdate = rotate(mat4(1.0f), radians(this->rotation.y), vec3(0.0f, 1.0f, 0.0f));
	mat4 scaleUpdate = scale(mat4(1.0f), vec3(1.0f + this->scaling));

	mat4 groupMatrix = worldRotationUpdate * translate(mat4(1.0f), this->position) * rotationUpdate * scaleUpdate;
	mat4 modelPart = groupMatrix * translate(mat4(1.0f), componentPosition) * part;

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &modelPart[0][0]);
	//mesh.draw(shaderProgram);
	glDrawArrays(this->renderMode, 0, 36);
}

