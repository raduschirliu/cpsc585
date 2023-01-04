#include "core/material/TextureMaterial.h"

#include "core/ShaderManager.h"

using glm::mat4;
using glm::vec3;
using std::shared_ptr;

TextureMaterial::TextureMaterial(shared_ptr<Texture> diffuse)
    : diffuse_(diffuse)
{
}

void TextureMaterial::Apply(mat4 model_matrix) const
{
    BasicMaterial::Apply(model_matrix);

    auto& shader = GetShader();
    diffuse_->Bind(Texture::Slot::kTexture0);
    shader.SetUniform("uTexture", 0);
}

ShaderProgram& TextureMaterial::GetShader() const
{
    return ShaderManager::Instance().GetProgram(
        ShaderManager::ShaderType::kTextured);
}