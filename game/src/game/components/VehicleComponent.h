#pragma once

#include <object_ptr.hpp>

#include "engine/input/InputService.h"
#include "engine/physics/PhysicsService.h"
#include "engine/scene/Component.h"
#include "engine/scene/OnUpdateEvent.h"
#include "engine/scene/Transform.h"

// Vehicle details
#include <physx/CommonVehicleFiles/SnippetVehicleHelpers.h>
#include <physx/CommonVehicleFiles/directdrivetrain/DirectDrivetrain.h>
#include <physx/CommonVehicleFiles/serialization/BaseSerialization.h>
#include <physx/CommonVehicleFiles/serialization/DirectDrivetrainSerialization.h>

#include "engine/physics/HelperUtils.h"
#include "vehicle2/PxVehicleAPI.h"

class VehicleComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physicsService_;
    jss::object_ptr<InputService> input_service_;

    /* variables for vehicle */

    // Where to retreat the data from for the vehicle.
    std::string g_vehicle_data_path_ =
        "resources/vehicle_data/";

    // The vehicle with direct drivetrain
    DirectDriveVehicle g_vehicle_;

    // Vehicle simulation needs a simulation context to store global parameters
    // of the simulation such as gravitational acceleration.
    PxVehiclePhysXSimulationContext g_vehicle_simulation_context_;

    // The mapping between PxMaterial and friction.
    PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions_[16];
    PxU32 gNbPhysXMaterialFrictions_ = 0;
    PxReal gPhysXDefaultMaterialFriction_ = 1.0f;

    std::string g_vehicle_name_;

    // for the car to be spawned.
    PxTransform pose;

    // commands the car has to follow.
    std::vector<Command> gCommands;

    // for functions.
  private:
    void ValidFileChecker();
    bool InitializeVehicle();
    void InitMaterialFrictionTable();

  public:
    // Getters

    // Setters
    inline void SetVehicleDataPath(const std::string& data_path)
    {
        g_vehicle_data_path_ = data_path;
    }

    inline void SetVehicleName(const std::string& vehicle_name)
    {
        g_vehicle_name_ = vehicle_name;
        g_vehicle_.setUpActor(*physicsService_->GetKScene(), pose,
                              g_vehicle_name_.c_str());
    }
};
