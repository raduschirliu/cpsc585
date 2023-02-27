#include "VehicleComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string;
using std::string_view;
using namespace physx;

static constexpr PxReal kDefaultMaterialFriction = 1.0f;
static constexpr const char* kVehicleDataPath = "resources/vehicle_data";
static constexpr const char* kBaseParamFileName = "Base.jsonc";
static constexpr const char* kDirectDriveParamFileName = "DirectDrive.jsonc";

void VehicleComponent::LoadParams()
{
    const bool success_base = readBaseParamsFromJsonFile(
        kVehicleDataPath, kBaseParamFileName, g_vehicle_.mBaseParams);
    ASSERT_MSG(success_base,
               "Must be able to load vehicle base params from JSON file");

    const bool success_drivertrain = readDirectDrivetrainParamsFromJsonFile(
        kVehicleDataPath, kDirectDriveParamFileName,
        g_vehicle_.mBaseParams.axleDescription, g_vehicle_.mDirectDriveParams);
    ASSERT_MSG(success_drivertrain,
               "Must be able to load vehicle drivetrain params from JSON file");

    setPhysXIntegrationParams(
        g_vehicle_.mBaseParams.axleDescription, gPhysXMaterialFrictions_,
        gNbPhysXMaterialFrictions_, kDefaultMaterialFriction,
        g_vehicle_.mPhysXParams);
}

void VehicleComponent::InitVehicle()
{
    const bool vehicle_init_status = g_vehicle_.initialize(
        *physics_service_->GetKPhysics(), PxCookingParams(PxTolerancesScale()),
        *physics_service_->GetKMaterial(), true);
    ASSERT_MSG(vehicle_init_status, "Vehicle must successfully initialize");

    g_vehicle_.mTransmissionCommandState.gear =
        PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

    // Set up the simulation context.
    // The snippet is set up with
    // a) z as the longitudinal axis
    // b) x as the lateral axis
    // c) y as the vertical axis.
    // d) metres  as the lengthscale.
    g_vehicle_simulation_context_.setToDefault();
    g_vehicle_simulation_context_.frame.lngAxis = PxVehicleAxes::ePosZ;
    g_vehicle_simulation_context_.frame.latAxis = PxVehicleAxes::eNegX;
    g_vehicle_simulation_context_.frame.vrtAxis = PxVehicleAxes::ePosY;
    g_vehicle_simulation_context_.scale.scale = 1.0f;
    g_vehicle_simulation_context_.gravity = physics_service_->GetGravity();
    g_vehicle_simulation_context_.physxScene = physics_service_->GetKScene();
    g_vehicle_simulation_context_.physxActorUpdateMode =
        PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;

    PxRigidBody* rigidbody = g_vehicle_.mPhysXState.physxActor.rigidBody;
    ASSERT_MSG(rigidbody, "Vehicle must have valid PhysX Actor RigidBody");

    rigidbody->userData = &GetEntity();
    const uint32_t num_shapes = rigidbody->getNbShapes();
    PxShape* shape = nullptr;

    // First shape is the vehicle body, the next 4 should be the wheels.
    // TODO: enabling collision for all shapes makes the vehicle get stuck in
    // the floor?
    for (uint32_t i = 0; i < 1; i++)
    {
        rigidbody->getShapes(&shape, 1, i);
        ASSERT_MSG(shape, "RigidBody Shape must be valid");

        shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
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
    gPhysXMaterialFrictions_[0].friction = 1.0f;
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

    InitMaterialFrictionTable();
    LoadParams();
    InitVehicle();
}

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    if (input_service_->IsKeyPressed(GLFW_KEY_F10))
    {
        LoadParams();
    }

    g_vehicle_.step(physics_service_->GetTimeStep(),
                    g_vehicle_simulation_context_);

    const PxTransform& pose =
        g_vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();

    // g_vehicle_.mPhysXState.physxActor.rigidBody->getLinearVelocity()
    //               .magnitude();

    const GlmTransform transform = PxToGlm(pose);
    transform_->SetPosition(transform.position);
    transform_->SetOrientation(transform.orientation);
}

void VehicleComponent::OnDestroy()
{
    g_vehicle_.destroy();
}

std::string_view VehicleComponent::GetName() const
{
    return "Vehicle";
}

void VehicleComponent::SetVehicleName(const string& vehicle_name)
{
    physx::PxQuat quat(
        transform_->GetOrientation().x, transform_->GetOrientation().y,
        transform_->GetOrientation().z, transform_->GetOrientation().w);

    PxTransform pose(GlmToPx(transform_->GetPosition()), quat);

    g_vehicle_name_ = vehicle_name;
    g_vehicle_.setUpActor(*physics_service_->GetKScene(), pose,
                          g_vehicle_name_.c_str());
}


DirectDriveVehicle& VehicleComponent::GetVehicle()
{
    return g_vehicle_;
}


void VehicleComponent::SetPlayerStateData(PlayerStateData& data)
{
    player_data_ = &data;

glm::vec3 VehicleComponent::GetPosition()
{
    return transform_->GetPosition();
}

glm::quat VehicleComponent::GetOrientation()
{
    return transform_->GetOrientation();

}