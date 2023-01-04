#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "core/gfx/Rect.h"

class Transform
{
  public:
    Transform();
    Transform(glm::vec3 position, float scale);

    void SetPosition(glm::vec3 position);
    void SetScale(float scale);
    
    // Rotate around axis relative to current transform
    void Rotate(float angle_rad, glm::vec3 axis);

    void LookAt(glm::vec3 target);
    void SetForwardDirection(glm::vec3 forward);
    void SetParent(std::shared_ptr<Transform> parent);
    std::shared_ptr<Transform> GetParent();

    glm::mat4 GetLocalToWorldMatrix() const;

    glm::vec3 GetRightDirection() const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetUpDirection() const;
    glm::vec3 GetForwardDirection() const;

  private:
    glm::vec3 position_;
    glm::vec3 forward_;
    glm::vec3 up_;
    float scale_;
    glm::mat4 model_matrix_;
    glm::mat4 translation_matrix_, scale_matrix_, rotation_matrix_;
    std::shared_ptr<Transform> parent_;

    void UpdateModelMatrix();
};