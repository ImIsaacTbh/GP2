#version 400 core

out vec4 FragColor;

void main()
{
	// Depth is automatically written to depth buffer
	FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}