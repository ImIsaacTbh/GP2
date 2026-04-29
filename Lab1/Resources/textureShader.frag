#version 400 core

in vec2 texCoord0;
in vec3 fragPos;
in vec3 N;

uniform sampler2D diffuse;

#define MAX_LIGHTS 4
uniform sampler2D shadowMap[MAX_LIGHTS];
uniform mat4 lightSpaceMatrix[MAX_LIGHTS];

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform int numLights;

uniform vec3 ambientColor;
uniform vec3 viewPos;

out vec4 FragColor;

float ShadowCalculation(int lightIndex)
{
    vec4 fragPosLightSpace = lightSpaceMatrix[lightIndex] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;
    if (projCoords.x < 0.0 || projCoords.x > 1.0) return 0.0;
    if (projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[lightIndex], 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    vec3 baseColor = texture(diffuse, texCoord0).rgb;
    vec3 norm = normalize(N);
    vec3 viewDir = normalize(viewPos - fragPos);

    // Ambient
    vec3 ambient = ambientColor * baseColor;

    // Accumulate lighting from all lights
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; i++)
    {
        vec3 lightDir = normalize(lightPos[i] - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseLight = diff * lightColor[i] * baseColor;

        // Specular
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = spec * lightColor[i] * baseColor;

        // Shadow for this light
        float shadow = ShadowCalculation(i);

        result += (1.0 - shadow) * (diffuseLight + specular);
    }

    FragColor = vec4(ambient + result, 1.0);
}
