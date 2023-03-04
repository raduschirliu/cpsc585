#include "engine/physics/Hitbox.h"

#include <glm/glm.hpp>
#include <iostream>

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/scene/Entity.h"
#include "game/components/VehicleComponent.h"

using glm::vec3;
using physx::PxActorFlag;
using physx::PxBoxGeometry;
using physx::PxShapeFlag;
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
    if (vehicle_)
    {
        SyncTransform();
    }
}

string_view Hitbox::GetName() const
{
    return "Hitbox";
}

void Hitbox::SetSize(const vec3& size)
{
    size_ = size;
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    PxBoxGeometry geometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
    shape_ = physics_service_->CreateShape(geometry);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

    dynamic_->attachShape(*shape_);
    dynamic_->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}

glm::vec3 Hitbox::GetSize()
{
    return size_;
}

physx::PxShape* Hitbox::GetShape()
{
    return shape_;
}