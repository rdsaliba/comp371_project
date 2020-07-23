#pragma once
#include <vector>
#include "Structs.h"
/// <summary>
/// Mesh implementation based from learn openGL: https://learnopengl.com/Model-Loading/Mesh
/// </summary>
class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	Mesh();
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void draw(int shaderProgram);
private:
	// render data 
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};