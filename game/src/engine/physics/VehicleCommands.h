#pragma once

#include "PxPhysicsAPI.h"

enum class VehicleGear
{
    kReverse = 0,
    kNeutral = 1,
    kForward = 2
};

struct VehicleCommand
{
    float front_brake;
    float rear_brake;
    float throttle;
    float steer;

    VehicleCommand()
    {
        front_brake = 0.0f;
        rear_brake = 0.0f;
        throttle = 0.0f;
        steer = 0.0f;
    }

    VehicleCommand(const physx::PxF32& in_front_brake,
                   const physx::PxF32& in_rear_brake,
                   const physx::PxF32& in_throttle,
                   const physx::PxF32& in_steer)
    {
        front_brake = in_front_brake;
        rear_brake = in_rear_brake;
        throttle = in_throttle;
        steer = in_steer;
    }

    VehicleCommand& operator+(const VehicleCommand& a)
    {
        front_brake = front_brake + a.front_brake;
        rear_brake = rear_brake + a.rear_brake;
        throttle = throttle + a.throttle;
        steer = steer + a.steer;

        return *this;
    }

    VehicleCommand& operator+=(const VehicleCommand& a)
    {
        front_brake += a.front_brake;
        rear_brake += a.rear_brake;
        throttle += a.throttle;
        steer += a.steer;

        return *this;
    }
};
