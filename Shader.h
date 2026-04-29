#pragma once

#include <string>
#include <GL/glew.h>
#include "transform.h"
#include <algorithm>
#include <vector>

class Shader
{
public:
    Shader(const std::string& filename);
    void SetLights(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors);
    void Bind();
    void Update(const Transform& transform, const Camera& camera, const glm::vec3& lightPos,
        const glm::vec3& lightColor, const glm::vec3& ambientColor,
        float specularStrength = 0.5f, bool hasNormalMap = true,
        const glm::vec3& basicColor = glm::vec3(0, 0, 0));
    std::string LoadShader(const std::string& fileName);
    void SetTexture(const std::string& name, GLuint textureID, int unit);
    void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
    GLuint CreateShader(const std::string& text, unsigned int type);
    ~Shader();

    GLuint program;
    GLuint shaders[NUM_SHADERS];

private:
    static const unsigned int NUM_SHADERS = 2;
    GLint uniforms[NUM_UNIFORMS];

    enum
    {
        TRANSFORM_U,        // MVP matrix (projection * view * model)
        MODEL_U,            // Model matrix (for world-space frag positions)
        LIGHT_SPACE_U,      // Light-space matrix (for shadow mapping)

        LIGHT_POS_U,
        LIGHT_COLOR_U,
        AMBIENT_COLOR_U,
        VIEW_POS_U,         // Camera world position

        SPECULAR_STRENGTH_U, // Per-draw specular control (was hardcoded 0.5)
        HAS_NORMAL_MAP_U,   // Replaces the fragile black-pixel check

        BASIC_COLOUR_U,

        // Texture units are bound via SetTexture() by name,
        // so we don't need individual uniform slots for them here.
        // Kept as named constants for documentation purposes.
        // DIFFUSE_TEX  -> unit 0, name "diffuse"
        // NORMAL_TEX   -> unit 1, name "normalMap"
        // EMISSION_TEX -> unit 2, name "emissionMap"
        // SHADOW_MAP   -> unit 3, name "shadowMap"

        NUM_UNIFORMS
    };
};