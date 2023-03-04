#include "VehicleComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string;
using std::string_view;
using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

static constexpr PxReal kDefaultMaterialFriction = 1.0f;
static constexpr const char* kVehicleDataPath = "resources/vehicle_data";
static constexpr const char* kBaseParamFileName = "Base.jsonc";
static constexpr const char* kDirectDriveParamFileName = "DirectDrive.jsonc";

void VehicleComponent::LoadParams()
{
    const bool success_base = readBaseParamsFromJsonFile(
        kVehicleDataPath, kBaseParamFileName, vehicle_.mBaseParams);
    ASSERT_MSG(success_base,
               "Must be able to load vehicle base params from JSON file");

    const bool success_drivertrain = readDirectDrivetrainParamsFromJsonFile(
        kVehicleDataPath, kDirectDriveParamFileName,
        vehicle_.mBaseParams.axleDescription, vehicle_.mDirectDriveParams);
    ASSERT_MSG(success_drivertrain,
               "Must be able to load vehicle drivetrain params from JSON file");

    setPhysXIntegrationParams(vehicle_.mBaseParams.axleDescription,
                              gPhysXMaterialFrictions_,
                              gNbPhysXMaterialFrictions_,
                              kDefaultMaterialFriction, vehicle_.mPhysXParams);
}

void VehicleComponent::InitVehicle()
{
    const bool vehicle_init_status = vehicle_.initialize(
        *physics_service_->GetKPhysics(), PxCookingParams(PxTolerancesScale()),
        *physics_service_->GetKMaterial(), true);
    ASSERT_MSG(vehicle_init_status, "Vehicle must successfully initialize");

    vehicle_.mTransmissionCommandState.gear =
        PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

    PxRigidBody* rigidbody = vehicle_.mPhysXState.physxActor.rigidBody;
    ASSERT_MSG(rigidbody, "Vehicle must have valid PhysX Actor RigidBody");

    rigidbody->userData = &GetEntity();
    rigidbody->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
    // rigidbody->setCMassLocalPose();
    const uint32_t num_shapes = rigidbody->getNbShapes();
    PxShape* shape = nullptr;

    // First shape is the vehicle body, the next 4 should be the wheels.
    for (uint32_t i = 0; i < num_shapes; i++)
    {
        rigidbody->getShapes(&shape, 1, i);
        ASSERT_MSG(shape, "RigidBody Shape must be valid");

        if (i == 0)
        {
            // TODO: enabling collision for all shapes makes the vehicle get
            // stuck in the floor?
            shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
            shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        }

        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
        shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
    }
}

void VehicleComponent::InitMaterialFrictionTable()
{
    // Each physx material can be mapped to a tire friction value on a per tire
    // basis. If a material is encountered that is not mapped to a friction
    // value, the friction value used is the specified default value. In this
    // snippet there is only a single material so there can only be a single
    // mapping between material and friction. In this snippet the same mapping
    // is used by all tires.
    gPhysXMaterialFrictions_[0].friction = 10.0f;
    gPhysXMaterialFrictions_[0].material = physics_service_->GetKMaterial();
    gNbPhysXMaterialFrictions_ = 1;
}

void VehicleComponent::OnInit(const ServiceProvider& service_provider)
{
    physics_service_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();
    transform_ = &GetEntity().GetComponent<Transform>();
    game_state_service_ = &service_provider.GetService<GameStateService>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);
    GetEventBus().Subscribe<OnPhysicsUpdateEvent>(this);

    InitMaterialFrictionTable();
    LoadParams();
    InitVehicle();

    physics_service_->RegisterVehicle(&vehicle_);
}

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F10))
    {
        LoadParams();
    }

    const PxTransform& pose =
        vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();

    const GlmTransform transform = PxToGlm(pose);
    transform_->SetPosition(transform.position);
    transform_->SetOrientation(transform.orientation);
}

void VehicleComponent::OnPhysicsUpdate(const Timestep& step)
{
}

void VehicleComponent::OnDestroy()
{
    physics_service_->UnregisterVehicle(&vehicle_);
    vehicle_.destroy();
}

std::string_view VehicleComponent::GetName() const
{
    return "Vehicle";
}

void VehicleComponent::SetVehicleName(const string& vehicle_name)
{
    const PxTransform pose = CreatePxTransform(transform_->GetPosition(),
                                               transform_->GetOrientation());

    g_vehicle_name_ = vehicle_name;
    vehicle_.setUpActor(*physics_service_->GetKScene(), pose,
                        g_vehicle_name_.c_str());
}

DirectDriveVehicle& VehicleComponent::GetVehicle()
{
    return vehicle_;
}

void VehicleComponent::SetPlayerStateData(PlayerStateData& data)
{
    player_data_ = &data;
}

glm::vec3 VehicleComponent::GetPosition()
{
    return transform_->GetPosition();
}

glm::quat VehicleComponent::GetOrientation()
{
    return transform_->GetOrientation();
}

PlayerStateData* VehicleComponent::GetPlayerStateData()
{
    if (player_data_)
        return player_data_;
    return nullptr;
}

