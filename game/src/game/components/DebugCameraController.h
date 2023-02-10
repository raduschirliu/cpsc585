#pragma once

#include <optional>

#include "engine/input/InputService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class DebugCameraController : public Component,
                              public IEventSubscriber<OnUpdateEvent>
{
  public:
    DebugCameraController();

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

  protected:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<InputService> input_service_;
    std::optional<glm::vec2> last_mouse_pos_;
    float pitch_degrees_;
    float yaw_degrees_;

  private:
    glm::vec3 GetMovementDir();
    void UpdateRotation(float pitch_delta_degrees, float yaw_delta_degrees);
};
