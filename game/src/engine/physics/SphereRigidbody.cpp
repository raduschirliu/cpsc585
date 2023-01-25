#include "engine/physics/SphereRigidbody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

void SphereRigidbody::OnInit(const ServiceProvider& service_provider)
{
    Log::info("SphereRigidbody - Init");

    physicsService_ = &service_provider.GetService<PhysicsService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    glm::vec3 starting_pos = transform_->GetPosition();

    dynamic_ = physicsService_->CreateRigidDynamic(
        starting_pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
    SetRadius(5.0f);
}

void SphereRigidbody::OnUpdate()
{
    // While the sphere is getting updated, update the transform location.
    if (dynamic_)
    {
        glm::vec3 updated_sphere_location = glm::vec3(
            dynamic_->getGlobalPose().p.x, dynamic_->getGlobalPose().p.y,
            dynamic_->getGlobalPose().p.z);
        transform_->SetPosition(updated_sphere_location);
    }
    // std::cout << transform_->GetPosition().y << std::endl;
}

string_view SphereRigidbody::GetName() const
{
    return "SphereRigidbody";
}

void SphereRigidbody::SetRadius(float radius)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    physx::PxSphereGeometry geometry(radius);
    shape_ = physicsService_->CreateShape(geometry);
    dynamic_->attachShape(*shape_);
}
