#include "game/components/BasicComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

PxRigidDynamic* Sphere;

void BasicComponent::OnInit(const ServiceProvider& service_provider)
{
    Log::info("BasicComponent - Init");

    physicsService_ = &service_provider.GetService<PhysicsService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    // creating a plane (along XZ, with normal pointing towards +Y)
    physicsService_->CreatePlaneRigidBody(PxPlane(0.0f, 1.0f, 0.0f, 0.0f));

    physx::PxVec3 conversion_transform =
        physx::PxVec3(transform_->GetPosition().x, transform_->GetPosition().y,
                      transform_->GetPosition().z);
    // creating a sphere and getting its dynamic which will be later used to
    // change the location of sphere.
    Sphere = physicsService_->CreateSphereRigidBody(
        3.0f, PxTransform(conversion_transform), 10.0f, PxVec3(0.0f));
}

void BasicComponent::OnUpdate()
{
    // While the sphere is getting updated, update the transform location.
    if (Sphere)
    {
        glm::vec3 updated_sphere_location =
            glm::vec3(Sphere->getGlobalPose().p.x, Sphere->getGlobalPose().p.y,
                      Sphere->getGlobalPose().p.z);
        transform_->SetPosition(updated_sphere_location);
    }
    // std::cout << transform_->GetPosition().y << std::endl;
}

string_view BasicComponent::GetName() const
{
    return "BasicComponent";
}
