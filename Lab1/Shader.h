#pragma once
#include <string>
#include <GL/glew.h>
#include "transform.h"
class Shader
{
public:

	Shader(const std::string& filename);

	void Bind(); //Set gpu to use our shaders
	void Update(const Transform& transform, const Camera& camera, const glm::vec3 lightingPos, const glm::vec3 basicColor = glm::vec3(0,0,0));

	std::string LoadShader(const std::string& fileName);

	void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);

	GLuint CreateShader(const std::string& text, unsigned int type);

	~Shader();

	GLuint program; // Track the shader program GLuint shaders[NUM_SHADERS]; 
protected:

private:
	static const unsigned int NUM_SHADERS = 2; // number of shaders
	enum
	{
		TRANSFORM_U,
		MODEL_U,
		LIGHT_POS_U,
		LIGHT_COLOR_U,
		AMBIENT_COLOR_U,
		BASIC_COLOUR_U,

		NUM_UNIFORMS
	};
	
	GLuint shaders[NUM_SHADERS]; //array of shaders
	GLuint uniforms[NUM_UNIFORMS]; //no of uniform variables

};
