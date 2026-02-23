#version 400 core

in vec3 position;
in vec2 texCoord;
in vec3 normal;

out vec3 ourColor;
out vec3 normalWorld;
out vec3 fragPos;

uniform mat4 transform;
uniform mat4 model;

void main()
{
	gl_Position = transform * vec4(position, 1.0);
	fragPos = vec3(model * vec4(position, 1.0));
	ourColor = vec3(texCoord, 1.0);
	normalWorld = normalize(mat3(transpose(inverse(model))) * normal);
}