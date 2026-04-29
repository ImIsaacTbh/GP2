#version 400 core

in vec3 position;
in vec2 texCoord;
in vec3 normal;

out vec2 texCoord0;
out vec3 fragPos;
out vec3 N;
out vec4 fragPosLightSpace;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = transform * vec4(position, 1.0);
    fragPos = vec3(model * vec4(position, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    N = normalize(normalMatrix * normal);

    texCoord0 = texCoord;
}
