#pragma once

#include "engine/scene/Component.h"
#include "engine/scene/Transform.h"

class RenderService;

class PointLight final : public Component
{
    void SetColor(const glm::vec3& color);
    const glm::vec3 GetColor() const;

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui();
    std::string_view GetName() const override;

  private:
    jss::object_ptr<RenderService> render_service_;
    jss::object_ptr<Transform> transform_;
    glm::vec3 color_ = glm::vec3(1.0f, 1.0f, 1.0f);
};
