#pragma once

#include "DebugCameraController.h"

class FollowCameraController final : public DebugCameraController
{
    public:
    void OnUpdate(const Timestep& delta_time) override;
};