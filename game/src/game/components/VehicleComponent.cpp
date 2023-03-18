#include "VehicleComponent.h"

#include <imgui.h>
#include <physx/CommonVehicleFiles/SnippetVehicleHelpers.h>
#include <physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.h>
#include <physx/CommonVehicleFiles/serialization/BaseSerialization.h>
#include <physx/CommonVehicleFiles/serialization/DirectDrivetrainSerialization.h>
#include <physx/CommonVehicleFiles/serialization/IntegrationSerialization.h>

#include <glm/geometric.hpp>

#include "engine/core/debug/Log.h"
#include "engine/core/gui/PropertyWidgets.h"
#include "engine/core/math/Physx.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Entity.h"
#include "game/components/audio/SoundEmitter.h"  // debugging

using glm::vec3;
using std::string;
using std::string_view;
using namespace physx;
using namespace physx::vehicle2;
using namespace snippetvehicle2;

static constexpr PxReal kDefaultMaterialFriction = 1.0f;
static constexpr const char* kVehicleDataPath = "resources/vehicle_data";
static constexpr const char* kBaseParamFileName = "Base.jsonc";
static constexpr const char* kDirectDriveParamFileName = "DirectDrive.jsonc";
static constexpr const char* kIntegrationParamFileName = "Integration.jsonc";

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

    const bool success_integration = readPhysxIntegrationParamsFromJsonFile(
        kVehicleDataPath, kIntegrationParamFileName, vehicle_.mPhysXParams);
    ASSERT_MSG(
        success_integration,
        "Must be able to load vehicle integration params from JSON file");

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
    gPhysXMaterialFrictions_[0].friction = 75.0f;
    gPhysXMaterialFrictions_[0].material = physics_service_->GetKMaterial();
    gNbPhysXMaterialFrictions_ = 1;
}

void VehicleComponent::OnInit(const ServiceProvider& service_provider)
{
    //    sound_emitter_ = &GetEntity().GetComponent<SoundEmitter>();  //
    //    debugging

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
        // sound_emitter_->PlaySource();
        LoadParams();
        Log::info("Reloaded vehicle params from JSON files...");
    }

    // std::cout << respawn_vehicle_ << std::endl;

    if (game_state_service_->GetRespawnRequested(this->GetEntity().GetId()))
    {
        vehicle_.mPhysXState.physxActor.rigidBody->setGlobalPose(
            CreatePxTransform(transform_->GetPosition(),
                              transform_->GetOrientation()));

        // now remove this from the list in gameservice so that it doesnt
        // respawn again and again until requested again later.
        game_state_service_->RemoveRespawnPlayers(this->GetEntity().GetId());
    }

       else
    {
        const PxTransform& pose =
            vehicle_.mPhysXState.physxActor.rigidBody->getGlobalPose();

        const GlmTransform transform = PxToGlm(pose);
        transform_->SetPosition(transform.position);
        transform_->SetOrientation(transform.orientation);
    }

    // std::cout << transform_->GetForwardDirection() << std::endl;
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

void VehicleComponent::OnDebugGui()
{
    ImGui::Text("Gear: %d", vehicle_.mTransmissionCommandState.gear);
    ImGui::Text("Steer: %f", vehicle_.mCommandState.steer);
    ImGui::Text("Throttle: %f", vehicle_.mCommandState.throttle);
    ImGui::Text("Front Brake: %f", vehicle_.mCommandState.brakes[0]);
    ImGui::Text("Rear Brake: %f", vehicle_.mCommandState.brakes[1]);

    const vec3 linear_velocity =
        PxToGlm(vehicle_.mBaseState.rigidBodyState.linearVelocity);
    gui::ViewProperty("Linear Velocity", linear_velocity);

    const float speed = glm::length(linear_velocity);
    ImGui::Text("Speed: %f", speed);

    auto vehicle_frame = vehicle_.mBaseParams.frame;
    const float lat_speed =
        vehicle_.mBaseState.rigidBodyState.getLateralSpeed(vehicle_frame);
    const float long_speed =
        vehicle_.mBaseState.rigidBodyState.getLongitudinalSpeed(vehicle_frame);

    ImGui::Text("Lat Speed: %f", lat_speed);
    ImGui::Text("Long Speed: %f", long_speed);

    const vec3 angular_velocity =
        PxToGlm(vehicle_.mBaseState.rigidBodyState.angularVelocity);
    gui::ViewProperty("Angular Velocity", angular_velocity);
}

DirectDriveVehicle& VehicleComponent::GetVehicle()
{
    return vehicle_;
}

void VehicleComponent::SetPlayerStateData(PlayerStateData& data)
{
    player_data_ = &data;
}

void VehicleComponent::SetGear(VehicleGear gear)
{
    PxVehicleDirectDriveTransmissionCommandState::Enum px_gear;
    switch (gear)
    {
        case VehicleGear::kReverse:
            px_gear =
                PxVehicleDirectDriveTransmissionCommandState::Enum::eREVERSE;
            break;

        case VehicleGear::kNeutral:
            px_gear =
                PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL;
            break;

        case VehicleGear::kForward:
            px_gear =
                PxVehicleDirectDriveTransmissionCommandState::Enum::eFORWARD;
            break;
    }

    vehicle_.mTransmissionCommandState.gear = px_gear;
}

void VehicleComponent::SetCommand(VehicleCommand command)
{
    vehicle_.mCommandState.throttle = glm::clamp(command.throttle, 0.0f, 1.0f);
    vehicle_.mCommandState.steer = glm::clamp(command.steer, -1.0f, 1.0f);

    vehicle_.mCommandState.nbBrakes = 2;
    vehicle_.mCommandState.brakes[0] =
        glm::clamp(command.front_brake, 0.0f, 1.0f);
    vehicle_.mCommandState.brakes[1] =
        glm::clamp(command.rear_brake, 0.0f, 1.0f);
}

VehicleGear VehicleComponent::GetGear() const
{
    switch (vehicle_.mTransmissionCommandState.gear)
    {
        case PxVehicleDirectDriveTransmissionCommandState::Enum::eREVERSE:
            return VehicleGear::kReverse;

        case PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL:
            return VehicleGear::kNeutral;

        case PxVehicleDirectDriveTransmissionCommandState::Enum::eFORWARD:
            return VehicleGear::kForward;

        default:
            ASSERT_ALWAYS(
                "Vehicle transmission should never be in an unkown state");
            return VehicleGear::kNeutral;
    }
}

float VehicleComponent::GetSpeed() const
{
    const vec3 velocity =
        PxToGlm(vehicle_.mPhysXState.physxActor.rigidBody->getLinearVelocity());
    return glm::length(velocity);
}
