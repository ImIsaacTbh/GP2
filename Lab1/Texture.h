#pragma once
#include <string>
#include <GL/glew.h>

class Texture
{
public:
	Texture(const std::string& fileName);

	std::string GetPath() const { return _path; }
	void Bind(unsigned int unit); // 32
	GLuint GetID() const { return textureHandler; }
	~Texture();

protected:
private:
	std::string _path;
	GLuint textureHandler;
};
