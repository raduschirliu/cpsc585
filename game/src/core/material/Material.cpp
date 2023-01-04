#include "core/material/Material.h"

#include "core/ShaderManager.h"

using glm::mat4;
using glm::vec3;

void BasicMaterial::Update(float delta_time)
{
}

void BasicMaterial::Apply(mat4 model_matrix) const
{
    ShaderProgram& shader = GetShader();

    shader.Use();
    shader.SetUniform("uModelMatrix", model_matrix);
}

ShaderProgram& BasicMaterial::GetShader() const
{
    return ShaderManager::Instance().GetProgram(
        ShaderManager::ShaderType::kBasic);
}
