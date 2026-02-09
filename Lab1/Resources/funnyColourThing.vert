#version 330 core
in vec3 aPos;
in vec2 texCoord;
out vec3 ourColor;
void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = vec3(texCoord, 1.0);
};