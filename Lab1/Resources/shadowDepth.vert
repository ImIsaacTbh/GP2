#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform mat4 lightSpaceMVP;

void main()
{
	gl_Position = lightSpaceMVP * vec4(position, 1.0);
}