#include "core/material/PhongLighting.h"

using glm::vec3;

static constexpr vec3 kSunlightColor(244.0f / 255.0f, 233.0f / 255.0f,
                                     155.0f / 255.0f);

void PhongLighting::Apply(ShaderProgram& shader)
{
    // Lighting currently calculated in world space, so light pos doesn't need
    // to be transformed
    shader.SetUniform("uLight.position", vec3(0.0f, 0.0f, 0.0f));
    shader.SetUniform("uLight.ambient", vec3(0.25f, 0.25f, 0.25f));
    shader.SetUniform("uLight.diffuse", kSunlightColor);
    shader.SetUniform("uLight.specular", kSunlightColor);
}