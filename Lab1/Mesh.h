#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "objLoader.h"

struct Vertex
{
public:
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord)
	{
		this->pos = pos;
		this->texCoord = texCoord;
		// initialize normal to zero instead of using an uninitialized value
		this->normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec3* GetPos() { return &pos; }
	glm::vec2* GetTexCoord() { return &texCoord; }
	glm::vec3* GetNormal() { return &normal; }

private:
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

class Mesh
{
public:
	Mesh();
	//Mesh(Vertex* vertices, unsigned int numVertices);
	~Mesh();

	void Draw();
	void Init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	void LoadModel(const std::string& filename);
	void InitModel(const objl::Mesh& model);
	void InitModel(const IndexedModel& model);

private:

	enum
	{
		POSITION_VERTEXBUFFER,
		TEXCOORD_VB,
		NORMAL_VB,
		INDEX_VB,
		NUM_BUFFERS
	};

	GLuint vertexArrayObject;
	GLuint vertexArrayBuffers[NUM_BUFFERS]; // create our array of buffers
	unsigned int drawCount; //how much of the vertexArrayObject do we want to draw
};