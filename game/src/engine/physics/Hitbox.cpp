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
    game_state_service_ = &service_provider.GetService<GameStateService>();
    RigidBodyComponent::OnInit(service_provider);
    vehicle_ = &GetEntity().GetComponent<VehicleComponent>();
    SetSize(kDefaultSize);
}

void Hitbox::OnUpdate(const Timestep& delta_time)
{
    if (vehicle_)
    {
        if (uint32_t id = game_state_service_->GetHitBoxMultiplier() != NULL)
        {
            if (GetEntity().GetId() != id)
            {
                // setting the size of the hitbox for all the other cars.
                SetSize(vec3(20.f, 20.f, 20.f));
            }
            else
            {
                // this is the entity which started the powerup, so do nothing.
            }
        }
        else
        {
            SetSize(kDefaultSize);
        }
        Log::debug("{}, {}, {}", size_.x, size_.y, size_.z);
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