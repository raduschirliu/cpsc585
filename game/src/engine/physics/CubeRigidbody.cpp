#include "engine/physics/CubeRigidbody.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;

using namespace physx;

void CubeRigidbody::OnInit(const ServiceProvider& service_provider)
{
    Log::info("CubeRigidbody - Init");

    physicsService_ = &service_provider.GetService<PhysicsService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    input_service_ = &service_provider.GetService<InputService>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
}

void CubeRigidbody::OnUpdate()
{
    if (b_can_control_)
    {
        // While the sphere is getting updated, update the transform location.
        if (input_service_->IsKeyDown(GLFW_KEY_UP))
        {
            transform_->Translate(glm::vec3(0.f, 0.f, -1.f));
        }
        if (input_service_->IsKeyDown(GLFW_KEY_DOWN))
        {
            transform_->Translate(glm::vec3(0.f, 0.f, 1.f));
        }
        if (input_service_->IsKeyDown(GLFW_KEY_LEFT))
        {
            transform_->Translate(glm::vec3(-1.f, 0.f, 0.f));
        }
        if (input_service_->IsKeyDown(GLFW_KEY_RIGHT))
        {
            transform_->Translate(glm::vec3(1.f, 0.f, 0.f));
        }
        // updating the position of the cube
        if (dynamic_)
        {
            dynamic_->setGlobalPose(CreateTransform(
                transform_->GetPosition(), glm::quat(1.f, 0.f, 0.f, 0.f)));
        }
    }
}

string_view CubeRigidbody::GetName() const
{
    return "CubeRigidbody";
}

void CubeRigidbody::SetHalfLength(float length_x, float length_y,
                                  float length_z)
{
    if (shape_)
    {
        dynamic_->detachShape(*shape_);
    }
    shape_ = physicsService_->CreateShapeCube(length_x, length_y, length_z);
    dynamic_->attachShape(*shape_);
}

void CubeRigidbody::CreateCube(float length_x, float length_y, float length_z)
{
    shape_ = physicsService_->CreateShapeCube(length_x, length_y, length_z);
    dynamic_ = physicsService_->CreateRigidDynamic(
        transform_->GetPosition(), glm::quat(1, 0, 0, 0), shape_);
}

bool CubeRigidbody::GetCanControl()
{
    return b_can_control_;
}

void CubeRigidbody::SetCanControl(bool bValue)
{
    b_can_control_ = bValue;
}
