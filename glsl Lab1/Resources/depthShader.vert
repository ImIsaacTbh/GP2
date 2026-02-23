#version 120

attribute vec3 position;
attribute vec3 normal;

varying vec3 normal0;
varying vec3 fragPos;

uniform mat4 transform;
uniform mat4 model;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    fragPos = vec3(model * vec4(position, 1.0));
    
    // Transform normal to world space
    normal0 = normalize(mat3(transpose(inverse(model))) * normal);
}