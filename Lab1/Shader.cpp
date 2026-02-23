#include "Shader.h"
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& filename)
{
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

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: link sadge");
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: shader bad :(");
	
	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform");
	uniforms[MODEL_U] = glGetUniformLocation(program, "model");
	uniforms[LIGHT_POS_U] = glGetUniformLocation(program, "lightPos");
	uniforms[LIGHT_COLOR_U] = glGetUniformLocation(program, "lightColor");
	uniforms[AMBIENT_COLOR_U] = glGetUniformLocation(program, "ambientColor");
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

void Shader::Update(const Transform& transform, const Camera& camera, const glm::vec3 lightPos)
{
	glm::mat4 mvp = camera.GetViewProjection() * transform.GetModel();
	glm::mat4 model = transform.GetModel();
	
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(uniforms[MODEL_U], 1, GL_FALSE, glm::value_ptr(model));
	
	// Set static lighting
		//Position
	glUniform3f(uniforms[LIGHT_POS_U], lightPos.x, lightPos.y, lightPos.z);
		//Light Colour
	glUniform3f(uniforms[LIGHT_COLOR_U], 1.0f, 1.0f, 1.0f);
		//Ambient Colour
	glUniform3f(uniforms[AMBIENT_COLOR_U], 0.2f, 0.2f, 0.2f);
}

void Shader::Bind()
{
	glUseProgram(program);
}

std::string Shader::LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

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
