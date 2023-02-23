#include "engine/physics/Hitbox.h"

#include <glm/glm.hpp>
#include <iostream>

#include "engine/core/math/Physx.h"
#include "engine/scene/Entity.h"
#include "game/components/VehicleComponent.h"
#include "engine/core/debug/Log.h"

using glm::vec3;
using physx::PxTransform;
using std::string_view;

static constexpr vec3 kDefaultSize(5.0f, 5.0f, 5.0f);
static constexpr float kDefaultDenisty = 10.0f;

void Hitbox::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    SetSize(kDefaultSize);
}

void Hitbox::OnUpdate(const Timestep& delta_time)
{
    if (vehicle_ /* is not null_ptr*/)  
    {
        transform_->SetPosition(vehicle_->GetPosition());
        transform_->SetOrientation(vehicle_->GetOrientation());
        SyncTransform();
    }
}

string_view Hitbox::GetName() const
{
    return "Hitbox";
}

void Hitbox::SetSize(const vec3& size)
{
    if (shape_)
        dynamic_->detachShape(*shape_);

    shape_ = physics_service_->CreateShapeCube(size.x, size.y, size.z);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    dynamic_->attachShape(*shape_);
    dynamic_->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}

// void Hitbox::SetFollow(Entity& entity)
// {
//     vehicle_ = &entity.GetComponent<VehicleComponent>();

//     // "transform_" from RigidBodyComponent
//     transform_->SetPosition(vehicle_->GetPosition());
//     transform_->SetOrientation(vehicle_->GetOrientation());
// }