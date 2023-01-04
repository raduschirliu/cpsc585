#include "core/material/EarthMaterial.h"

#include "core/ShaderManager.h"
#include "core/Utils.h"
#include "core/gfx/Log.h"
#include "core/material/PhongLighting.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using std::shared_ptr;

static constexpr float kCloudSpeed = 0.15f;

EarthMaterial::EarthMaterial(shared_ptr<Texture> diffuse,
                             shared_ptr<Texture> diffuse_night,
                             shared_ptr<Texture> diffuse_clouds,
                             shared_ptr<Texture> specular, float shininess)
    : diffuse_(diffuse),
      diffuse_night_(diffuse_night),
      diffuse_clouds_(diffuse_clouds),
      specular_(specular),
      shininess_(shininess),
      time_(0.0f)
{
}

void EarthMaterial::Update(float delta_time)
{
    time_ += utils::kTimeScale * delta_time * kCloudSpeed;
}

void EarthMaterial::Apply(mat4 model_matrix) const
{
    BasicMaterial::Apply(model_matrix);

    auto& shader = GetShader();
    PhongLighting::Apply(shader);

    diffuse_->Bind(Texture::Slot::kTexture0);
    diffuse_night_->Bind(Texture::Slot::kTexture1);
    diffuse_clouds_->Bind(Texture::Slot::kTexture2);
    specular_->Bind(Texture::Slot::kTexture3);

    shader.SetUniform("uMaterial.diffuse", 0);
    shader.SetUniform("uMaterial.diffuseNight", 1);
    shader.SetUniform("uMaterial.diffuseClouds", 2);
    shader.SetUniform("uMaterial.specular", 3);
    shader.SetUniform("uMaterial.shininess", shininess_);

    shader.SetUniform("uTime", time_);
}

ShaderProgram& EarthMaterial::GetShader() const
{
    return ShaderManager::Instance().GetProgram(
        ShaderManager::ShaderType::kEarth);
}