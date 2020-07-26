#include "Sphere.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/// <summary>
/// Sphere implementation inspired from: http://www.songho.ca/opengl/gl_sphere.html#:~:text=In%20order%20to%20draw%20the,triangle%20strip%20cannot%20be%20used.
/// </summary>
Sphere::Sphere() {
	this->radius = 0;
	this->sectorCtr = 0;
	this->stackCtr = 0;
	this->vao = -1;
	this->vbo = -1;
}

Sphere::Sphere(float radius, int sectorCtr, int stackCtr, int shaderProgram) {
	this->radius = radius;
	this->sectorCtr = sectorCtr;
	this->stackCtr = stackCtr;
	this->shaderProgram = shaderProgram;
}

void Sphere::draw(int shaderProgram, mat4 sphere) {
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &sphere[0][0]);
	glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
}

void Sphere::buildSphere() {
	const float PI = acos(-1);

	float sectorStep = (2 * PI) / sectorCtr;
	float stackStep = PI / stackCtr;
	float sectorAngle, stackAngle;
	vector<TexturedColoredVertex> tempVertices;

	//generates all vertices of the sphere
	for (int i = 0; i <= stackCtr; ++i) {
		stackAngle = (PI / 2) - (i * stackStep);
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCtr; ++j) {

			TexturedColoredVertex vertex;
			vertex.position = getVerticePosition(i, j);
			vertex.uv = getVerticeUV(i, j);
			vertex.color = vec3(0.0f, 1.0f, 1.0f);
			tempVertices.push_back(vertex);
		}
	}

	int i, j, k, vi1, vi2;
	TexturedColoredVertex v1, v2, v3, v4;
	for (i = 0; i < stackCtr; ++i) {
		vi1 = i * (sectorCtr + 1);
		vi2 = (i + 1) * (sectorCtr + 1);
		
		for (j = 0; j < sectorCtr; ++j, ++vi1, ++vi2) {
			v1 = tempVertices[vi1];
			v2 = tempVertices[vi2];
			v3 = tempVertices[vi1 + 1];
			v4 = tempVertices[vi2 + 1];

			if (i == 0) {
				computeFaceNormal(&v1, &v2, &v4, NULL);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v4);
			}
			else if (i == (stackCtr - 1)) {
				computeFaceNormal(&v1, &v2, &v3, NULL);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);
			}
			else {
				computeFaceNormal(&v1, &v2, &v3, &v4);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);
				vertices.push_back(v4);
			}
		}
	}
}

vec3 Sphere::getVerticePosition(int currentStack, int currentSector) {
	const float PI = acos(-1);

	float sectorStep = (2 * PI) / sectorCtr;
	float stackStep = PI / stackCtr;
	float stackAngle = (PI / 2) - (currentStack * stackStep);
	float xy = radius * cosf(stackAngle);
	float z = radius * sinf(stackAngle);
	float sectorAngle = currentSector * sectorStep;

	vec3 position;
	position.x = xy * cosf(sectorAngle);
	position.y = xy * sinf(sectorAngle);
	position.z = z;

	return position;
}

vec2 Sphere::getVerticeUV(int currentStack, int currentSector) {
	vec2 uv;
	uv.x = (float)currentSector / sectorCtr;
	uv.y = (float)currentStack / stackCtr;

	return uv;
}

void Sphere::computeFaceNormal(TexturedColoredVertex* v1, TexturedColoredVertex* v2, TexturedColoredVertex* v3, TexturedColoredVertex* v4) {
	const float EPSILON = 0.000001f;

	vec3 normalVector(0.0f);
	float normalX, normalY, normalZ;

	//Edge Vectors 
	//v1-v2
	float edgeX1 = v2->position.x - v1->position.x;
	float edgeY1 = v2->position.y - v1->position.y;	
	float edgeZ1 = v2->position.z - v1->position.z;
	//v1-v3
	float edgeX2 = v3->position.x - v1->position.x;
	float edgeY2 = v3->position.y - v1->position.y;
	float edgeZ2 = v3->position.z - v1->position.z;

	//cross product
	normalX = (edgeY1 * edgeZ2) - (edgeZ1 * edgeY2);
	normalY = (edgeY1 * edgeX2) - (edgeX1 * edgeZ2);
	normalZ = (edgeX1 * edgeY2) - (edgeY1 * edgeX2);

	float length = sqrtf((normalX * normalX) + (normalY * normalY) + (normalZ * normalZ));
	if (length > EPSILON) {
		float lengthInv = 1.0f / length;
		normalVector.x = normalX * lengthInv;
		normalVector.y = normalY * lengthInv;
		normalVector.z = normalZ * lengthInv;
	}

	v1->normal = normalVector;
	v2->normal = normalVector;
	v3->normal = normalVector;
	if(v4 != NULL)
		v4->normal = normalVector;
}