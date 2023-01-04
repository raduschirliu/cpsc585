#include "core/entity/Transform.h"

#include <glm/gtx/transform.hpp>

#include "core/Utils.h"

using glm::mat4;
using glm::vec3;
using glm::vec4;
using std::shared_ptr;
using utils::kIdentityMatrix;

Transform::Transform()
    : position_(vec3(0.0f, 0.0f, 0.0f)),
      forward_(vec3(0.0f, 0.0f, -1.0f)),
      up_(vec3(0.0f, 1.0f, 0.0f)),
      scale_(1.0f),
      model_matrix_(kIdentityMatrix),
      translation_matrix_(kIdentityMatrix),
      scale_matrix_(kIdentityMatrix),
      rotation_matrix_(kIdentityMatrix),
      parent_(nullptr)
{
}

Transform::Transform(vec3 position, float scale) : Transform()
{
    SetPosition(position);
    SetScale(scale);
}

void Transform::SetPosition(vec3 position)
{
    position_ = position;
    translation_matrix_ = glm::translate(position);

    UpdateModelMatrix();
}

void Transform::SetScale(float scale)
{
    scale_ = scale;
    scale_matrix_ = glm::scale(vec3(scale_, scale_, 0.0f));

    UpdateModelMatrix();
}

void Transform::Rotate(float angle_rad, vec3 axis)
{
    rotation_matrix_ *= glm::rotate(angle_rad, axis);
    UpdateModelMatrix();
}

void Transform::LookAt(vec3 target)
{
    vec3 dir = target - position_;

    if (glm::length(dir) > 0)
    {
        // Not updating model matrix since we don't care about rotation
        forward_ = glm::normalize(dir);
    }
}

void Transform::SetParent(shared_ptr<Transform> parent)
{
    parent_ = parent;
}

shared_ptr<Transform> Transform::GetParent()
{
    return parent_;
}

mat4 Transform::GetLocalToWorldMatrix() const
{
    if (parent_)
    {
        return parent_->GetLocalToWorldMatrix() * model_matrix_;
    }
    else
    {
        return model_matrix_;
    }
}

void Transform::SetForwardDirection(vec3 forward)
{
    forward_ = glm::normalize(forward);
}

vec3 Transform::GetRightDirection() const
{
    constexpr vec3 up(0.0f, 1.0f, 0.0f);
    return glm::normalize(glm::cross(forward_, up));
}

void Transform::UpdateModelMatrix()
{
    // Scale -> Rotate -> Translate
    model_matrix_ = translation_matrix_ * rotation_matrix_ * scale_matrix_;
}

vec3 Transform::GetPosition() const
{
    return position_;
}

vec3 Transform::GetUpDirection() const
{
    return up_;
}

vec3 Transform::GetForwardDirection() const
{
    return forward_;
}