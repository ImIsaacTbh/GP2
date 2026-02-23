#version 120

varying vec3 normal0;
varying vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;

void main()
{
    vec3 n = normalize(normal0);
    
    // Calculate light direction from fragment to static light position
    vec3 l = normalize(lightPos - fragPos);
    
    // Diffuse lighting
    float diff = max(dot(n, l), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Ambient lighting
    vec3 ambient = ambientColor;
    
    // Combine lighting
    vec3 result = ambient + diffuse;
    
    gl_FragColor = vec4(result, 1.0);
}