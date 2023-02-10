#include "VehicleComponent.h"

#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"

using std::string_view;
using namespace physx;

void VehicleComponent::ValidFileChecker()
{
    // Check that we can read from the json file before continuing.
    BaseVehicleParams base_params;
    if (!readBaseParamsFromJsonFile(g_vehicle_data_path_.c_str(), "Base.json",
                                    base_params))
        ASSERT_MSG(false,
                   "Cannot open Base.json file, error in VehicleComponent.");

    // Check that we can read from the json file before continuing.
    DirectDrivetrainParams direct_drivetrain_params;
    if (!readDirectDrivetrainParamsFromJsonFile(
            g_vehicle_data_path_.c_str(), "DirectDrive.json",
            base_params.axleDescription, direct_drivetrain_params))
    {
        ASSERT_MSG(
            false,
            "Cannot open DirectDrive.json file, error in VehicleComponent.");
    }
}

bool VehicleComponent::InitializeVehicle()
{
    readBaseParamsFromJsonFile(g_vehicle_data_path_.c_str(), "Base.json",
                               g_vehicle_.mBaseParams);
    setPhysXIntegrationParams(
        g_vehicle_.mBaseParams.axleDescription, gPhysXMaterialFrictions_,
        gNbPhysXMaterialFrictions_, gPhysXDefaultMaterialFriction_,
        g_vehicle_.mPhysXParams);
    readDirectDrivetrainParamsFromJsonFile(
        g_vehicle_data_path_.c_str(), "DirectDrive.json",
        g_vehicle_.mBaseParams.axleDescription, g_vehicle_.mDirectDriveParams);

    ASSERT_MSG(g_vehicle_.initialize(*physicsService_->GetKPhysics(),
                                     PxCookingParams(PxTolerancesScale()),
                                     *physicsService_->GetKMaterial(), true),
               "Vehicle must successfully initialize");

    g_vehicle_.mTransmissionCommandState.gear =
        PxVehicleDirectDriveTransmissionCommandState::eFORWARD;

    pose = CreatePxTransform(transform_->GetPosition(),
                             glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

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
    g_vehicle_simulation_context_.gravity = physicsService_->GetGravity();
    g_vehicle_simulation_context_.physxScene = physicsService_->GetKScene();
    g_vehicle_simulation_context_.physxActorUpdateMode =
        PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
    return true;
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
    gPhysXMaterialFrictions_[0].material = physicsService_->GetKMaterial();
    gPhysXDefaultMaterialFriction_ = 1.0f;
    gNbPhysXMaterialFrictions_ = 1;
}

void VehicleComponent::OnInit(const ServiceProvider& service_provider)
{
    physicsService_ = &service_provider.GetService<PhysicsService>();
    input_service_ = &service_provider.GetService<InputService>();
    transform_ = &GetEntity().GetComponent<Transform>();

    GetEventBus().Subscribe<OnUpdateEvent>(this);

    ValidFileChecker();

    InitMaterialFrictionTable();
    ASSERT_MSG(InitializeVehicle(), "Vehicle must initialize");
}

void VehicleComponent::OnUpdate(const Timestep& delta_time)
{
    Command command_to_execute = {0.1f, 0.0f, 0.0f, 0.0f};

    // Input service so that we can add the commands to it
    if (input_service_->IsKeyDown(GLFW_KEY_UP))
    {
        Command temp = {0.0f, 1.0f, 0.0f, physicsService_->GetTimeStep()};
        command_to_execute = temp;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_LEFT))
    {
        Command temp = {0.0f, 0.1f, -0.1f, physicsService_->GetTimeStep()};
        command_to_execute = temp;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_RIGHT))
    {
        Command temp = {0.0f, 0.1f, 0.1f, physicsService_->GetTimeStep()};
        command_to_execute = temp;
    }
    else if (input_service_->IsKeyDown(GLFW_KEY_DOWN))
    {
        Command temp = {0.5f, 0.0f, 0.0f, physicsService_->GetTimeStep()};
        command_to_execute = temp;
    }

    g_vehicle_.mCommandState.brakes[0] = command_to_execute.brake;
    g_vehicle_.mCommandState.nbBrakes = 1;
    g_vehicle_.mCommandState.throttle = command_to_execute.throttle;
    g_vehicle_.mCommandState.steer = command_to_execute.steer;

    g_vehicle_.step(physicsService_->GetTimeStep(),
                    g_vehicle_simulation_context_);

    transform_->SetPosition(
        glm::vec3(g_vehicle_.mBaseState.rigidBodyState.pose.p.x,
                  g_vehicle_.mBaseState.rigidBodyState.pose.p.y,
                  g_vehicle_.mBaseState.rigidBodyState.pose.p.z));

    // transform_->SetOrientation(glm::quat(g_vehicle_.mBaseState.rigidBodyState.pose.q.w,
    //                             g_vehicle_.mBaseState.rigidBodyState.pose.q.x,
    //                             g_vehicle_.mBaseState.rigidBodyState.pose.q.y,
    //                             g_vehicle_.mBaseState.rigidBodyState.pose.q.z));

    // std::cout<<"vehicle : " << transform_->GetPosition() << std::endl;

    // Log::debug(" Car Position : {}, {}, {}", transform_->GetPosition().x,
    // transform_->GetPosition().y,  transform_->GetPosition().z);
}

std::string_view VehicleComponent::GetName() const
{
    return "Vehicle";
}
