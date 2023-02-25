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
    input_service_ = &service_provider.GetService<InputService>();

    // component dependencies
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void RaycastComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_R))
    {
        // origin and direction of the raycast from this entity
        glm::vec3 direction = transform_->GetForwardDirection();
        glm::vec3 origin = transform_->GetPosition();

        // no raycast data == no hit
        if (!physics_service_->Raycast(origin, direction).has_value())
        {
            return;
        }

        // get the data from the raycast hit
        RaycastData raycast =
            physics_service_->Raycast(origin, direction).value();

        physx::PxActor* actor = raycast.actor;
        float distance = raycast.distance;
        glm::vec3 normal = raycast.normal;
        glm::vec3 position = raycast.position;

        std::cout << "target actor: " << actor << "\n"
                  << "target position: " << position << "\t"
                  << "distance from target: " << distance << "\t"
                  << "normal of raycast hit: " << normal << "\t" << std::endl;
    }
}

std::string_view RaycastComponent::GetName() const
{
    return "RaycastComponent";
}