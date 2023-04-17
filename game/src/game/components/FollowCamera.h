#pragma once

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

class RenderService;
class InputService;
class VehicleComponent;
class PlayerState;

class FollowCamera : public Component, public IEventSubscriber<OnUpdateEvent>
{
  public:
    FollowCamera();

    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnDebugGui() override;
    std::string_view GetName() const override;

    // From IEventSubscriber<OnUpdateEvent>
    void OnUpdate(const Timestep& delta_time) override;

    // Set the entity from which we want to follow using this camera
    void SetFollowingTransform(Entity& entity);
    void SetCameraOffset(glm::vec3 offset);

    void SetPlayerState(PlayerState& player_state);

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<Transform> target_transform_;
    jss::object_ptr<VehicleComponent> target_vehicle_;

    // to get the player controller attached to the entity on which follow
    // camera is attached
    jss::object_ptr<PlayerState> player_state_;

    float acceleration_factor_;
    glm::vec3 offset_;
    glm::vec3 lookat_offset_;
    float distance_;
    float orientation_lerp_factor_;
    float position_lerp_factor_;
};