#pragma once
#include <vector>
#include <GL/glew.h> 
#include "Structs.h"
/// <summary>
/// Sphere implementation inspired from: http://www.songho.ca/opengl/gl_sphere.html#:~:text=In%20order%20to%20draw%20the,triangle%20strip%20cannot%20be%20used.
/// </summary>
class Sphere {
public:
	Sphere();
	Sphere(float radius, int sectorCtr, int stackCtr, int shaderProgram, vec3 position);
	~Sphere() {};

	float getRadius() { return radius; }
	int getSectorCtr() { return sectorCtr; }
	int getStackCtr() { return stackCtr; }

	unsigned int getVerticesSize() const { return (unsigned int)vertices.size() * sizeof(TexturedColoredVertex); } 
	unsigned int getVertexCount() const {return (unsigned int)vertices.size();}
	TexturedColoredVertex* getVertices() { return vertices.data(); }
	vec3 getPosition() { return position; }

	void setRadius(float radius) { this->radius = radius; }
	void setSectorCtr(int sectorCtr) { this->sectorCtr = sectorCtr; }
	void setStackCtr(int stackCtr) { this->stackCtr = stackCtr; }
	void setVao(unsigned int vao) { this->vao = vao; }
	void setVbo(unsigned int vbo) { this->vbo = vbo; }
	
	void buildSphere();
	void draw(int shaderProgram, mat4 sphere);

	//Translation values
	void x(float value) { this->position.x += value; }
	void y(float value) { this->position.y += value; }
	void z(float value) { this->position.z += value; }
private:
	float radius;
	int sectorCtr;
	int stackCtr;
	int shaderProgram;
	unsigned int vao;
	unsigned int vbo;
	vector<TexturedColoredVertex> vertices;
	vec3 position;
	vec3 getVertexPosition(int currentStack, int currentSector);
	vec2 getVertexUV(int currentStack, int currentSector);
	void computeFaceNormal(TexturedColoredVertex* v1, TexturedColoredVertex* v2, TexturedColoredVertex* v3, TexturedColoredVertex* v4);
};