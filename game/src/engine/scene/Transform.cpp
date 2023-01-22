#include "engine/scene/Transform.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;

Transform::Transform()
    : position_(0.0f, 0.0f, 0.0f),
      orientation_(vec3(0.0f, 0.0f, 0.0f)),
      forward_dir_(0.0f, 0.0f, 1.0f),
      scale_(1.0f, 1.0f, 1.0f),
      translation_matrix_(1.0f),
      rotation_matrix_(1.0f),
      scale_matrix_(1.0f),
      model_matrix_(1.0f)
{
}

void Transform::Translate(const vec3& delta)
{
    position_ += delta;
    UpdateMatrices();
}

void Transform::SetPosition(const vec3& position)
{
    position_ = position;
    UpdateMatrices();
}

const vec3& Transform::GetPosition() const
{
    return position_;
}

const vec3& Transform::GetForwardDir() const
{
    return forward_dir_;
}

const mat4& Transform::GetModelMatrix() const
{
    return model_matrix_;
}

void Transform::OnInit(const ServiceProvider& service_provider)
{
}

std::string_view Transform::GetName() const
{
    return "Transform";
}

void Transform::UpdateMatrices()
{
    translation_matrix_ = glm::translate(mat4(1.0f), position_);
    rotation_matrix_ = glm::toMat4(orientation_);
    scale_matrix_ = glm::scale(mat4(1.0f), scale_);

    model_matrix_ = translation_matrix_ * rotation_matrix_ * scale_matrix_;
}