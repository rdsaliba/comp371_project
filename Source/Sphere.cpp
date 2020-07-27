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
	this->position = vec3(0.0f);
}

Sphere::Sphere(float radius, int sectorCtr, int stackCtr, int shaderProgram, vec3 position) {
	this->radius = radius;
	this->sectorCtr = sectorCtr;
	this->stackCtr = stackCtr;
	this->shaderProgram = shaderProgram;
	this->position = position;
	this->vao = -1;
}

/// <summary>
/// Draws a Sphere with provided transformations
/// </summary>
/// <param name="shaderProgram"></param>
/// <param name="sphere">Matrix containing transformations to apply onto a basic sphere</param>
void Sphere::draw(int shaderProgram, mat4 sphere) {
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &sphere[0][0]);
	glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
}

/// <summary>
/// Generates all vertices of a sphere
/// </summary>
void Sphere::buildSphere() {
	const float PI = acos(-1);

	float sectorStep = (2 * PI) / sectorCtr;
	float stackStep = PI / stackCtr;
	float sectorAngle, stackAngle;
	vector<TexturedColoredVertex> tempVertices;

	//Computes all vertices, unordered
	for (int i = 0; i <= stackCtr; ++i) {
		stackAngle = (PI / 2) - (i * stackStep);
		float xy = radius * cosf(stackAngle);
		float z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCtr; ++j) {

			TexturedColoredVertex vertex;
			vertex.position = getVertexPosition(i, j);
			vertex.uv = getVertexUV(i, j);
			vertex.color = vec3(0.0f, 1.0f, 1.0f);
			tempVertices.push_back(vertex);
		}
	}

	//Computes the normal of each vertex & inserts them in an orderly fashion
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

			//The sectors of the initial stack is only composed of triangles
			if (i == 0) {
				computeFaceNormal(&v1, &v2, &v4, NULL);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v4);
			}
			//Sectors on the last stack is like the first stack, only composed of triangles
			else if (i == (stackCtr - 1)) {
				computeFaceNormal(&v1, &v2, &v3, NULL);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);
			}
			//4 vertices per sector, forming squares
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

/// <summary>
/// Computes the position of a vertex within a sphere
///
/// The algorithm to calculate the positions was taken from: http://www.songho.ca/opengl/gl_sphere.html#:~:text=In%20order%20to%20draw%20the,triangle%20strip%20cannot%20be%20used.
/// </summary>
/// <param name="currentStack"></param>
/// <param name="currentSector"></param>
/// <returns></returns>
vec3 Sphere::getVertexPosition(int currentStack, int currentSector) {
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

/// <summary>
/// Calculates the aUV texture coordinate of a vertex
/// </summary>
/// <param name="currentStack"></param>
/// <param name="currentSector"></param>
/// <returns></returns>
vec2 Sphere::getVertexUV(int currentStack, int currentSector) {
	vec2 uv;
	uv.x = (float)currentSector / sectorCtr;
	uv.y = (float)currentStack / stackCtr;

	return uv;
}

/// <summary>
/// Computes the Normal vector of vertices based on their position within the sphere
///
/// This function is heavily inspired by: http://www.songho.ca/opengl/gl_sphere.html#:~:text=In%20order%20to%20draw%20the,triangle%20strip%20cannot%20be%20used.
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="v3"></param>
/// <param name="v4"></param>
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

	//Applies normal onto given vertices
	v1->normal = normalVector;
	v2->normal = normalVector;
	v3->normal = normalVector;

	//Only sectors that are not in the first or last stack (both tips of the sphere) may have 4 vertices
	//In that case, the 4th vertex will have the same normal as the 3 other ones.
	if(v4 != NULL)
		v4->normal = normalVector;
}