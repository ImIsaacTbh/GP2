#version 400 core

in vec3 normalWorld;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 basicColor;

out vec4 FragColor;

void main()
{
	vec3 n = normalize(normalWorld);
	vec3 l = normalize(lightPos - fragPos);
	
	float diff = max(dot(n, l), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 ambient = ambientColor;
	
	FragColor = vec4(basicColor * (ambient + diffuse), 1.0);
}