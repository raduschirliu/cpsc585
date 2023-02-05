#include "engine/physics/CubeRigidbody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using glm::vec3;
using physx::PxTransform;
using std::string_view;

void CubeRigidbody::OnInit(const ServiceProvider& service_provider)
{
    Log::info("CubeRigidbody - Init");

    physics_service_ = &service_provider.GetService<PhysicsService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    PxTransform pose = CreatePxTransform(transform_->GetPosition(),
                                       transform_->GetOrientation());
    dynamic_ = physics_service_->GetKPhysics()->createRigidDynamic(pose);
    PxRigidBodyExt::updateMassAndInertia(*dynamic_, 10.0f);

    physics_service_->RegisterDynamicActor(dynamic_);
}

void CubeRigidbody::OnUpdate(const Timestep& delta_time)
{
    const PxTransform pose = dynamic_->getGlobalPose();
    const GlmTransform transform = PxToGlm(pose);
    transform_->SetPosition(transform.position);
    transform_->SetOrientation(transform.orientation);
}

string_view CubeRigidbody::GetName() const
{
    return "CubeRigidbody";
}

void CubeRigidbody::SetSize(const vec3& size)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }

    shape_ = physics_service_->CreateShapeCube(size.x, size.y, size.z);
    dynamic_->attachShape(*shape_);
}
