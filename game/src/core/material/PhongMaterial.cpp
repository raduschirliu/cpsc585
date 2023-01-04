#include "core/material/PhongMaterial.h"

#include "core/ShaderManager.h"
#include "core/material/PhongLighting.h"

using glm::mat4;
using glm::vec3;
using std::shared_ptr;

static constexpr vec3 kSunlightColor(244.0f / 255.0f, 233.0f / 255.0f,
                                     155.0f / 255.0f);

PhongMaterial::PhongMaterial(shared_ptr<Texture> diffuse, vec3 specular,
                             float shininess)
    : diffuse_(diffuse),
      specular_(specular),
      shininess_(shininess)
{
}

void PhongMaterial::Apply(mat4 model_matrix) const
{
    BasicMaterial::Apply(model_matrix);

    auto& shader = GetShader();

    PhongLighting::Apply(shader);

    diffuse_->Bind(Texture::Slot::kTexture0);

    shader.SetUniform("uMaterial.diffuse", 0);
    shader.SetUniform("uMaterial.specular", specular_);
    shader.SetUniform("uMaterial.shininess", shininess_);
}

ShaderProgram& PhongMaterial::GetShader() const
{
    return ShaderManager::Instance().GetProgram(
        ShaderManager::ShaderType::kPhong);
}