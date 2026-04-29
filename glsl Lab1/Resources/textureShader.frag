#version 400 core

in vec2 texCoord0;
in vec3 fragPos;
in vec3 N;
in vec4 fragPosLightSpace;

uniform sampler2D diffuse;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 viewPos;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;
    if(projCoords.x < 0.0 || projCoords.x > 1.0) return 0.0;
    if(projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
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

    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // Ambient
    vec3 ambient = ambientColor * baseColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColor * baseColor;

    // Specular - multiply by baseColor so viewPos feeds into final colour
    // and cannot be optimised out by the GLSL compiler
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor * baseColor;

    // Shadow
    float shadow = ShadowCalculation(fragPosLightSpace);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuseLight + specular);

    FragColor = vec4(lighting, 1.0);
}
