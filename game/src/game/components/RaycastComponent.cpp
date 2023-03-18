#include "game/components/RaycastComponent.h"

#include <stdio.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>

#include "PxPhysics.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/Hitbox.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

static constexpr size_t kGamepadId = GLFW_JOYSTICK_1;

using glm::vec3;

void RaycastComponent::OnInit(const ServiceProvider& service_provider)
{
    debug::LogInfo("RaycastComponent - Init");

    // service dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    hitbox_ = &GetEntity().GetComponent<Hitbox>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void RaycastComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_R) ||
        input_service_->IsGamepadButtonPressed(kGamepadId,
                                               GLFW_GAMEPAD_BUTTON_B))
    {
        Shoot();
    }
}

std::string_view RaycastComponent::GetName() const
{
    return "RaycastComponent";
}

void RaycastComponent::Shoot()
{
    // origin and direction of the raycast from this entity
    vec3 direction = transform_->GetForwardDirection();
    vec3 offset = 15.f * direction;
    vec3 origin = transform_->GetPosition() + offset;

    // no raycast data == no hit
    if (!physics_service_->Raycast(origin, direction).has_value())
    {
        return;
    }

    // get the data from the raycast hit
    RaycastData raycast = physics_service_->Raycast(origin, direction).value();

    float distance = raycast.distance;

    physx::PxActor* target_actor = raycast.actor;
    vec3 normal = raycast.normal;
    vec3 position = raycast.position;

    std::cout << "target actor: " << target_actor->getName() << "\n"
              << "target position: " << glm::to_string(position) << "\t"
              << "distance from target: " << distance << "\t"
              << "normal of raycast hit: " << glm::to_string(normal) << "\t"
              << std::endl;
}