#include "engine/physics/Hitbox.h"

using glm::vec3;
using physx::PxTransform;
using std::string_view;

static constexpr vec3 kDefaultSize(5.0f, 5.0f, 5.0f);

void Hitbox::OnInit(const ServiceProvider& service_provider)
{
    RigidBodyComponent::OnInit(service_provider);

    SetSize(kDefaultSize);
}

string_view Hitbox::GetName() const
{
    return "BoxTrigger";
}

void Hitbox::SetSize(const vec3& size)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    shape_ = physics_service_->CreateShapeCube(size.x, size.y, size.z);
    shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    dynamic_->attachShape(*shape_);
    dynamic_->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
}