#include "Shader.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& filename)
{
	_path = filename;
	program = glCreateProgram();
	shaders[0] = CreateShader(LoadShader(filename + ".vert"), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(filename + ".frag"), GL_FRAGMENT_SHADER);

	for (int i = 0; i < NUM_SHADERS; i++)
	{
		glAttachShader(program, shaders[i]);
	}

	glBindAttribLocation(program, 0, "position");
	glBindAttribLocation(program, 1, "texCoord");
	glBindAttribLocation(program, 2, "normal");
	glBindAttribLocation(program, 3, "tangent");
	glBindAttribLocation(program, 4, "bitangent");

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: link sadge");
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: shader bad :(");
	
	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform");
	uniforms[MODEL_U] = glGetUniformLocation(program, "model");
	uniforms[LIGHT_POS_U] = glGetUniformLocation(program, "lightPos");
	uniforms[LIGHT_COLOR_U] = glGetUniformLocation(program, "lightColor");
	uniforms[AMBIENT_COLOR_U] = glGetUniformLocation(program, "ambientColor");
	uniforms[BASIC_COLOUR_U] = glGetUniformLocation(program, "basicColor");
	uniforms[VIEW_POS_U] = glGetUniformLocation(program, "viewPos");
	uniforms[LIGHT_SPACE_MATRIX_U] = glGetUniformLocation(program, "lightSpaceMatrix");
	uniforms[DIFFUSE_TEX_U] = glGetUniformLocation(program, "diffuseMap");
	uniforms[NORMAL_TEX_U] = glGetUniformLocation(program, "normalMap");
	uniforms[EMISSION_TEX_U] = glGetUniformLocation(program, "emissionMap");
	GLint specStrengthLoc = glGetUniformLocation(program, "specularStrength");
	//for debug
	std::cout << "specularStrength location: " << specStrengthLoc << std::endl;
	std::cout << "Shader program " << filename << " loaded with ID: " << program << std::endl;
	std::cout << "  lightPos location: " << uniforms[LIGHT_POS_U] << std::endl;
	std::cout << "  lightColor location: " << uniforms[LIGHT_COLOR_U] << std::endl;
	std::cout << "  viewPos location: " << uniforms[VIEW_POS_U] << std::endl;
	std::cout << "  ambientColor location: " << uniforms[AMBIENT_COLOR_U] << std::endl;
	std::cout << "  viewPos location: " << uniforms[VIEW_POS_U] << std::endl;

}

Shader::~Shader()
{
	for (int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	glDeleteProgram(program);
}

void Shader::SetTexture(const std::string& uniformname, GLuint textureID, int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLint loc = glGetUniformLocation(program, uniformname.c_str());
	if (loc != -1)
	{
		glUniform1i(loc, unit);
	}
}

void Shader::Update(const Transform& transform, const Camera& camera, const glm::vec3 lightPos, const glm::vec3 basicColor)
{
	glm::mat4 mvp = camera.GetViewProjection() * transform.GetModel();
	glm::mat4 model = transform.GetModel();
	
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniforms[MODEL_U], 1, GL_FALSE, glm::value_ptr(model));
	
	// Set static lighting
	glUniform3f(uniforms[LIGHT_POS_U], lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(uniforms[LIGHT_COLOR_U], 1.0f, 1.0f, 1.0f);
	glUniform3f(uniforms[AMBIENT_COLOR_U], 0.2f, 0.2f, 0.2f);
	glUniform3f(uniforms[BASIC_COLOUR_U], basicColor.x, basicColor.y, basicColor.z);
	
	// Set view position
	if (uniforms[VIEW_POS_U] != -1)
	{
		glm::vec3 camPos = camera.GetPosition();
		glUniform3f(uniforms[VIEW_POS_U], camPos.x, camPos.y, camPos.z);
	}
}

void Shader::SetLights(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors)
{
	int count = std::min((int)positions.size(), 4); // cap at MAX_LIGHTS

	for (int i = 0; i < count; i++)
	{
		std::string posName = "lightPos[" + std::to_string(i) + "]";
		std::string colName = "lightColor[" + std::to_string(i) + "]";

		GLint posLoc = glGetUniformLocation(program, posName.c_str());
		GLint colLoc = glGetUniformLocation(program, colName.c_str());

		if (posLoc != -1) glUniform3f(posLoc, positions[i].x, positions[i].y, positions[i].z);
		if (colLoc != -1) glUniform3f(colLoc, colors[i].x, colors[i].y, colors[i].z);
	}

	GLint numLoc = glGetUniformLocation(program, "numLights");
	if (numLoc != -1) glUniform1i(numLoc, count);
}

void Shader::UpdateWithShadow(const Transform& transform, const Camera& camera, const glm::vec3 lightPos, const glm::mat4 lightSpaceMatrix, const glm::vec3 basicColor)
{
	glm::mat4 mvp = camera.GetViewProjection() * transform.GetModel();
	glm::mat4 model = transform.GetModel();
	
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniforms[MODEL_U], 1, GL_FALSE, glm::value_ptr(model));
	
	if (uniforms[LIGHT_SPACE_MATRIX_U] != -1)
	{
		glUniformMatrix4fv(uniforms[LIGHT_SPACE_MATRIX_U], 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	}
	
	// Set lighting uniforms
	glUniform3f(uniforms[LIGHT_POS_U], lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(uniforms[LIGHT_COLOR_U], 1.0f, 1.0f, 1.0f);
	glUniform3f(uniforms[AMBIENT_COLOR_U], 0.2f, 0.2f, 0.2f);
	//glUniform3f(uniforms[AMBIENT_COLOR_U], 1.0f, 1.0f, 1.0f); debug 
	glUniform3f(uniforms[BASIC_COLOUR_U], basicColor.x, basicColor.y, basicColor.z);
	
	if (uniforms[VIEW_POS_U] != -1)
	{
		glm::vec3 camPos = camera.GetPosition();
		glUniform3f(uniforms[VIEW_POS_U], camPos.x, camPos.y, camPos.z);
	}

	GLint hasNormalMapLoc = glGetUniformLocation(program, "hasNormalMap");
	if (hasNormalMapLoc != -1)
		glUniform1i(hasNormalMapLoc, 0); // 0 = false, no normal map by default

	GLint specStrengthLoc = glGetUniformLocation(program, "specularStrength");
	if (specStrengthLoc != -1)
		glUniform1f(specStrengthLoc, 0.5f);
}

void Shader::Bind()
{
	glUseProgram(program);
}

void Shader::SetLightSpaceMatrices(const std::vector<glm::mat4>& matrices)
{
	for (int i = 0; i < matrices.size() && i < 4; i++)
	{
		std::string name = "lightSpaceMatrix[" + std::to_string(i) + "]";
		GLint loc = glGetUniformLocation(program, name.c_str());
		if (loc != -1)
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrices[i]));
	}
}

std::string Shader::LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());
	std::cout << "Loading shader: " << fileName << (file.is_open() ? " OK" : " FAILED") << std::endl;
	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cout << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

GLuint Shader::CreateShader(const std::string& text, unsigned int type)
{
	GLuint shader = glCreateShader(type);

	if (shader == 0)
		std::cerr << "Error type creation failed " << type << std::endl;

	const GLchar* stringSource[1];
	stringSource[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, stringSource, lengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

	return shader;
}
