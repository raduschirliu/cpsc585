#pragma once

#include "PxPhysicsAPI.h"

struct Command
{
    physx::PxF32 brake;
    physx::PxF32 throttle;
    physx::PxF32 steer;
    physx::PxF32 duration;

    // The normal constructor, brake is applied automatically as car needs to
    // slow down when nothing happens
    Command()
    {
        brake = 0.1f;
        throttle = 0.f;
        steer = 0.f;
        duration = 0.f;
    }

    Command(const physx::PxF32& in_brake, const physx::PxF32& in_throttle,
            const physx::PxF32& in_steer, const physx::PxF32& in_duration)
    {
        brake = in_brake;
        throttle = in_throttle;
        steer = in_steer;
        duration = in_duration;
    }

    Command& operator+(const Command& a)
    {
        brake = brake + a.brake;
        throttle = throttle + a.throttle;
        steer = steer + a.steer;

        // not adding the duration as it is not required.
        return *this;
    }

    Command& operator+=(const Command& a)
    {
        brake += a.brake;
        throttle += a.throttle;
        steer += a.steer;

        // not adding the duration as it is not required.
        return *this;
    }
};
