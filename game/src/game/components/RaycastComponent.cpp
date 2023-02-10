#include "game/components/RaycastComponent.h"

#include <stdio.h>

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "PxPhysics.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void RaycastComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("RaycastComponent - Init");

    // dependencies
    transform_ = &GetEntity().GetComponent<Transform>();
    input_service_ = &service_provider.GetService<InputService>();
    physics_service_ = &service_provider.GetService<PhysicsService>();

    // events
    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void RaycastComponent::OnUpdate(const Timestep& delta_time)
{
    // mouse right-click to raycast
    if (!input_service_->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
        return;

    /*
    note: rn object hits itself with its own cast
    when i change direction to upDirection and shoot immediately
    i get a hit for a split second and then never again lol
    */
    glm::vec3 origin = transform_->GetPosition();
    glm::vec3 direction = transform_->GetForwardDirection();

    // no raycast data == no hit
    if (!physics_service_->Raycast(origin, direction).has_value())
        return;

    // get data from raycast hit:
    RaycastData raycast = physics_service_->Raycast(origin, direction).value();
    float distance = raycast.distance;
    glm::vec3 normal = raycast.normal;
    glm::vec3 position = raycast.position;

    // idk how to Log::debug this lol
    std::cout << "distance: " << distance << "\t"
              << "normal: " << normal << "\t"
              << "position: " << position << "\t" << std::endl;
}

std::string_view RaycastComponent::GetName() const
{
    return "RaycastComponent";
}