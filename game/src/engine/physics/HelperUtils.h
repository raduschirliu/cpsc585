#pragma once
#include "PxPhysicsAPI.h"

struct Command
{
    physx::PxF32 brake;
    physx::PxF32 throttle;
    physx::PxF32 steer;
    physx::PxF32 duration;
};
