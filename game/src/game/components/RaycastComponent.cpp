#include "game/components/RaycastComponent.h"

#include <stdio.h>

#include <glm/glm.hpp>
#include <object_ptr.hpp>

#include "PxPhysics.h"
#include "engine/core/debug/Log.h"
#include "engine/input/InputService.h"
#include "engine/physics/BoxRigidBody.h"
#include "engine/physics/PhysicsService.h"
#include "engine/physics/RaycastData.h"
#include "engine/scene/Component.h"
#include "engine/scene/Entity.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

void RaycastComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("RaycastComponent - Init");

    // service dependencies
    physics_service_ = &service_provider.GetService<PhysicsService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    // ok so i realized vehicles don't have box rigid bodies
    // box_rigid_body_ = &GetEntity().GetComponent<BoxRigidBody>();
    // self_shape_ = box_rigid_body_->GetShape();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void RaycastComponent::OnUpdate(const Timestep& delta_time)
{
    // right-click to raycast
    // if (!input_service_->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
    //     return;

    // origin and direction of the raycast from this entity
    glm::vec3 direction = transform_->GetForwardDirection();
    glm::vec3 origin = transform_->GetPosition();

    std::cout << "forward direction: " << direction << std::endl;

    // no raycast data == no hit
    if (!physics_service_->Raycast(origin, direction).has_value())
        return;

    // get the data from the raycast hit
    RaycastData raycast = physics_service_->Raycast(origin, direction).value();

    // physx::PxActor* self_actor = self_shape_->getActor();
    physx::PxActor* actor = raycast.actor;

    // raycast ignores own entity
    // if (actor == self_actor)
    // return;

    float distance = raycast.distance;
    glm::vec3 normal = raycast.normal;
    glm::vec3 position = raycast.position;

    std::cout << "distance: " << distance << "\t"
              << "normal: " << normal << "\t"
              << "position: " << position << "\t"
              << "actor: " << actor << "\t" << std::endl;
}

std::string_view RaycastComponent::GetName() const
{
    return "RaycastComponent";
}