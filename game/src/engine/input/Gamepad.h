#pragma once

#include <GLFW/glfw3.h>

#include <array>
#include <string>

#include "engine/input/InputState.h"

class Gamepad
{
  public:
    // Gamepad axis values will be ignored if abs(val) < threshold
    static constexpr float kAxisThreshold = 0.1f;

    static constexpr size_t kGamepadCount = GLFW_JOYSTICK_LAST + 1;
    static constexpr size_t kAxisCount = GLFW_GAMEPAD_AXIS_LAST + 1;
    static constexpr size_t kButtonCount = GLFW_GAMEPAD_BUTTON_LAST + 1;

    static const std::string& GetAxisName(int axis);

    Gamepad(int id);

    bool Connect();
    void Disconnect();
    void UpdateState();

    int GetId() const;
    bool GetActive() const;
    float GetAxis(int axis) const;
    bool IsButtonDown(int button) const;
    bool IsButtonPressed(int button) const;
    const std::string& GetName() const;

  private:
    int id_;
    bool active_;
    std::string name_;
    GLFWgamepadstate raw_state_;
    std::array<ButtonState, kButtonCount> buttons_;

    void UpdateButtonState(size_t button);
};
