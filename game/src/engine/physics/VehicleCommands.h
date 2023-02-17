#pragma once

#include "PxPhysicsAPI.h"

struct Command
{
    physx::PxF32 brake;
    physx::PxF32 throttle;
    physx::PxF32 steer;
    physx::PxF32 duration;

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
