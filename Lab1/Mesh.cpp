#include "Mesh.h"

#include <algorithm>
#include <vector>

Mesh::Mesh()
{

}

Mesh::Mesh(std::string shaderPath, std::string texturePath, std::string bumpMap, std::string emissionMap)
{
	normalTex = nullptr;
	emissionTex = nullptr;
	diffuseTex = nullptr;
	if(!shaderPath.empty())
		shader = new Shader(shaderPath);
	if(!texturePath.empty())
		diffuseTex = new Texture(texturePath);
	//if (!bumpMap.empty())
	//	normalTex = new Texture(bumpMap);
	//if (!emissionMap.empty())
	//	emissionTex = new Texture(emissionMap);
}

//Mesh::Mesh(Vertex* vertices, unsigned int numVertices)
//{
//	Mesh::drawCount = numVertices;
//	std::vector<glm::vec3> positions;
//	std::vector<glm::vec2> texCoords;
//	positions.reserve(numVertices);
//	texCoords.reserve(numVertices);
//
//	for (int i = 0; i < numVertices; i++)
//	{
//		positions.push_back(*(vertices[i].GetPos()));
//		texCoords.push_back(*(vertices[i].GetTexCoord()));
//	}
//
//	glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);
//	//vertex positions
//	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VERTEXBUFFER]);
//	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	//texture coord shit
//	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
//	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
//
//	//bind those 2 mfs
//	glBindVertexArray(0);
//}

Mesh::~Mesh()
{
	//cleanup
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(NUM_BUFFERS, vertexArrayBuffers);
}

void Mesh::Init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	IndexedModel model;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
		model.normals.push_back(*vertices[i].GetNormal());
	}

	for (unsigned int i = 0; i < numIndices; i++)
	{
		model.indices.push_back(indices[i]);
	}
	InitModel(model);
}

void Mesh::LoadModel(const std::string& filename)
{
	/*IndexedModel model = OBJModel(filename).ToIndexedModel();*/
	objl::Loader loader;
	loader.LoadFile(filename);
	InitModel(loader.LoadedMeshes[0]);
	InitMaterials(loader.LoadedMaterials);
}

void Mesh::InitMaterials(const std::vector<objl::Material> mats)
{
	for each(auto mat in mats)
	{
		materials.push_back(mat);
	}
}

void Mesh::InitModel(const objl::Mesh& model)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<std::vector<glm::vec3>> mtlCoords;

	positions.reserve(model.Vertices.size());
	texCoords.reserve(model.Vertices.size());
	normals.reserve(model.Vertices.size());

	for (size_t i = 0; i < model.Vertices.size(); ++i)
	{
		const auto& v = model.Vertices[i];
		positions.emplace_back(v.Position.X, v.Position.Y, v.Position.Z);
		texCoords.emplace_back(v.TextureCoordinate.X, -v.TextureCoordinate.Y);
		normals.emplace_back(v.Normal.X, v.Normal.Y, v.Normal.Z);
	}

	drawCount = static_cast<unsigned int>(model.Indices.size());

	glGenVertexArrays(1, &vertexArrayObject); 
	glBindVertexArray(vertexArrayObject); 

	glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VERTEXBUFFER]);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.Indices.size() * sizeof(model.Indices[0]), &model.Indices[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void Mesh::InitModel(const IndexedModel& model)
{
	drawCount = model.indices.size();

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, vertexArrayBuffers);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[POSITION_VERTEXBUFFER]);
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[TEXCOORD_VB]);
	
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(vertexArrayObject);

	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
