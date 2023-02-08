#include "Camera.h"

class FollowCamera : public Camera
{
    virtual void UpdateViewMatrix() override;
};