#version 400 core

in vec3 normalWorld;
in vec3 fragPos;

//uniform vec3 lightPos;
//uniform vec3 lightColor;
#define MAX_LIGHTS 4

uniform sampler2D shadowMap[MAX_LIGHTS];
uniform mat4 lightSpaceMatrix[MAX_LIGHTS];

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];

uniform vec3 ambientColor;
uniform vec3 basicColor;

uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
{
    vec3 baseColor = basicColor;
    //vec3 norm = normalize(N);
    vec3 viewDir = normalize(viewPos - fragPos);

    // Ambient
    vec3 ambient = ambientColor * baseColor;

    // Accumulate lighting from all lights
    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);

        // Diffuse
        vec3 diffuseLight = lightColor[i] * baseColor;

        result += diffuseLight;
    }

    FragColor = vec4(ambient + result, 1.0);
}