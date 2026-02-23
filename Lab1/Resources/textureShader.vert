#version 400 core

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec2 texCoord0;
out vec3 normal0;
out vec3 fragPos;

uniform mat4 transform;
uniform mat4 model;

void main()
{
	gl_Position = transform * vec4(position, 1.0);
	fragPos = vec3(model * vec4(position, 1.0));
	texCoord0 = texCoord;
	normal0 = normalize(mat3(transpose(inverse(model))) * normal);
}