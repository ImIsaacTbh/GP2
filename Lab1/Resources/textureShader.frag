#version 400 core

in vec2 texCoord0;
in vec3 normal0;
in vec3 fragPos;

uniform sampler2D diffuse;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

out vec4 FragColor;

void main()
{
	vec3 n = normalize(normal0);
	vec3 l = normalize(lightPos - fragPos);
	
	float diff = max(dot(n, l), 0.0);
	vec3 diffuse_light = diff * lightColor;
	vec3 ambient = ambientColor;
	
	vec4 tex = texture(diffuse, texCoord0);
	FragColor = vec4(tex.rgb * (ambient + diffuse_light), tex.a);
}