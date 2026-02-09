#include "Shader.h"
#include <iostream>
#include <fstream>

Shader::Shader(const std::string& filename)
{
	program = glCreateProgram();
	//shaders[0] = CreateShader(LoadShader("C:\\Users\\aster\\source\\repos\\GP2-Lab1\\Lab1\\Resources\\funnyColourThing.vert"), GL_VERTEX_SHADER);
	//shaders[1] = CreateShader(LoadShader("C:\\Users\\aster\\source\\repos\\GP2-Lab1\\Lab1\\Resources\\funnyColourThing.frag"), GL_FRAGMENT_SHADER);
	shaders[0] = CreateShader(LoadShader(filename + ".vert"), GL_VERTEX_SHADER);
	shaders[1] = CreateShader(LoadShader(filename + ".frag"), GL_FRAGMENT_SHADER);

	for (int i = 0; i < sizeof(shaders); i++)
	{
		glAttachShader(program, shaders[i]);
	}
	glBindAttribLocation(program, 0, "position");
	glBindAttribLocation(program, 1, "texCoord");

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Erorr: link sadge");
	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Eror: shader bad :(");
	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform");
}

Shader::~Shader()
{
	for (int i = 0; i < sizeof(shaders); i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}
	glDeleteProgram(program);
}

void Shader::Update(const Transform& transform)
{
	glm::mat4 model = transform.GetModel();
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GLU_FALSE, &model[0][0]);
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
	GLuint shader = glCreateShader(type); //create shader based on specified type

	if (shader == 0) //if == 0 shader no created
		std::cerr << "Error type creation failed " << type << std::endl;

	const GLchar* stringSource[1]; //convert strings into list of c-strings
	stringSource[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, stringSource, lengths); //send source code to opengl
	glCompileShader(shader); //get open gl to compile shader code

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!"); //check for compile error

	return shader;
}
