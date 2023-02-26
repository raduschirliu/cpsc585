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

#include "engine/physics/VehicleCommands.h"
#include "vehicle2/PxVehicleAPI.h"

class VehicleComponent final : public Component,
                               public IEventSubscriber<OnUpdateEvent>
{
  public:
    // From Component
    void OnInit(const ServiceProvider& service_provider) override;
    void OnUpdate(const Timestep& delta_time) override;
    void OnDestroy() override;
    std::string_view GetName() const override;

  private:
    jss::object_ptr<Transform> transform_;
    jss::object_ptr<PhysicsService> physics_service_;
    jss::object_ptr<InputService> input_service_;

    /* variables for vehicle */

    // The vehicle with direct drivetrain
    DirectDriveVehicle g_vehicle_;

    // Vehicle simulation needs a simulation context to store global parameters
    // of the simulation such as gravitational acceleration.
    PxVehiclePhysXSimulationContext g_vehicle_simulation_context_;

    // The mapping between PxMaterial and friction.
    PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions_[16];
    PxU32 gNbPhysXMaterialFrictions_ = 0;

    std::string g_vehicle_name_;

    // commands the car has to follow.
    std::vector<Command> gCommands;

    // for functions.
  private:
    void InitVehicle();
    void InitMaterialFrictionTable();
    void LoadParams();

  public:
    // Getters
    glm::vec3 GetPosition();
    glm::quat GetOrientation();

    inline DirectDriveVehicle& GetVehicle()
    {
        return g_vehicle_;
    }

    void SetVehicleName(const std::string& vehicle_name);
};
