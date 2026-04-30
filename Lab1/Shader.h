#pragma once
#include <string>
#include <GL/glew.h>
#include "transform.h"
#include <vector>

class Shader
{
public:

	Shader(const std::string& filename);

	std::string GetPath() const { return _path; }
	void Bind();
	void Update(const Transform& transform, const Camera& camera, const glm::vec3 lightingPos, const glm::vec3 basicColor = glm::vec3(0, 0, 0));
	void UpdateWithShadow(const Transform& transform, const Camera& camera, const glm::vec3 lightingPos, const glm::mat4 lightSpaceMatrix, const glm::vec3 basicColor = glm::vec3(0, 0, 0));
	void SetLights(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors);
	std::string LoadShader(const std::string& fileName);
	void SetTexture(const std::string& name, GLuint textureID, int unit);
	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	void SetLightSpaceMatrices(const std::vector<glm::mat4>& matrices);

	GLuint CreateShader(const std::string& text, unsigned int type);

	~Shader();

	GLuint program;

protected:

private:
	std::string _path;
	static const unsigned int NUM_SHADERS = 2;
	enum
	{
		TRANSFORM_U,
		MODEL_U,
		LIGHT_POS_U,
		LIGHT_COLOR_U,
		AMBIENT_COLOR_U,
		BASIC_COLOUR_U,
		VIEW_POS_U,
		LIGHT_SPACE_MATRIX_U,
		DIFFUSE_TEX_U,
		NORMAL_TEX_U,
		EMISSION_TEX_U,

		NUM_UNIFORMS
	};

	GLuint shaders[NUM_SHADERS];
	GLuint uniforms[NUM_UNIFORMS];
};
