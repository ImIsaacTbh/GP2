#version 330 core
in vec3 aPos;
in vec2 texCoord;
out vec3 ourColor;
uniform mat4 transform;
void main()
{
	gl_Position = transform * vec4(aPos, 1.0);
	ourColor = vec3(texCoord, 1.0);
};