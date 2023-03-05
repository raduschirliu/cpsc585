#include "engine/input/Gamepad.h"

#include "engine/core/debug/Assert.h"

using std::array;
using std::string;

/***************************************/
/***             Statics             ***/
/***************************************/

static const string kInactiveGamepadName = "$inactive";

static constexpr array<int, 4> kThresholdAxes = {
    GLFW_GAMEPAD_AXIS_LEFT_X,
    GLFW_GAMEPAD_AXIS_LEFT_Y,
    GLFW_GAMEPAD_AXIS_RIGHT_X,
    GLFW_GAMEPAD_AXIS_RIGHT_Y,
};

static const array<string, Gamepad::kAxisCount> kAxisNames = {
    "LeftX", "LeftY", "RightX", "RightY", "LeftTrigger", "RightTrigger"};

/***************************************/
/***          Class Members          ***/
/***************************************/

Gamepad::Gamepad(int id)
    : id_(id),
      active_(false),
      name_(kInactiveGamepadName),
      raw_state_(),
      buttons_{}
{
}

bool Gamepad::Connect()
{
    if (!glfwJoystickIsGamepad(id_))
    {
        return false;
    }

    active_ = true;
    name_ = glfwGetGamepadName(id_);

    return true;
}

void Gamepad::Disconnect()
{
    active_ = false;
    name_ = kInactiveGamepadName;
}

void Gamepad::UpdateState()
{
    if (!active_)
    {
        return;
    }

    glfwGetGamepadState(id_, &raw_state_);

    // Get axes and apply thresholds
    for (auto axis_idx : kThresholdAxes)
    {
        auto& axis_val = raw_state_.axes[axis_idx];
        if (abs(axis_val) < kAxisThreshold)
        {
            axis_val = 0.0f;
        }
    }

    // Buttons
    for (size_t i = 0; i < buttons_.size(); i++)
    {
        UpdateButtonState(i);
    }
}

const string& Gamepad::GetAxisName(int axis)
{
    ASSERT_MSG(axis < kAxisNames.size(), "Axis ID must be valid");

    return kAxisNames[axis];
}

int Gamepad::GetId() const
{
    return id_;
}

float Gamepad::GetAxis(int axis) const
{
    if (axis >= kAxisCount || axis < 0)
    {
        ASSERT_ALWAYS("Invalid gamepad axis");
        return 0.0f;
    }

    if (!active_)
    {
        return 0.0f;
    }

    return raw_state_.axes[axis];
}

bool Gamepad::IsButtonDown(int button) const
{
    if (button < 0 || button >= kButtonCount)
    {
        ASSERT_ALWAYS("Invalid gamepad button");
        return false;
    }

    if (!active_)
    {
        return false;
    }

    return buttons_[button] == ButtonState::kDown ||
           buttons_[button] == ButtonState::kPressed;
}

bool Gamepad::IsButtonPressed(int button) const
{
    if (button < 0 || button >= kButtonCount)
    {
        ASSERT_ALWAYS("Invalid gamepad button");
        return false;
    }

    if (!active_)
    {
        return false;
    }

    return buttons_[button] == ButtonState::kPressed;
}

const string& Gamepad::GetName() const
{
    return name_;
}

void Gamepad::UpdateButtonState(size_t button)
{
    if (raw_state_.buttons[button] == GLFW_PRESS)
    {
        switch (buttons_[button])
        {
            // Pressed previous frame, now held down
            case ButtonState::kPressed:
                buttons_[button] = ButtonState::kDown;
                break;

            // Already held down for 2+ frames
            case ButtonState::kDown:
                break;

            // Was previously depressed, but just pressed this frame for
            // first time
            case ButtonState::kReleased:
            case ButtonState::kUp:
                buttons_[button] = ButtonState::kPressed;
                break;

            default:
                ASSERT_ALWAYS("This ButtonState should never happen");
        };
    }
    else
    {
        switch (buttons_[button])
        {
            // Pressed previous frame, now released
            case ButtonState::kPressed:
            case ButtonState::kDown:
                buttons_[button] = ButtonState::kReleased;
                break;

            // Was released last frame
            case ButtonState::kReleased:
                buttons_[button] = ButtonState::kUp;
                break;

            // Already up for 2+ frames
            case ButtonState::kUp:
                break;

            default:
                ASSERT_ALWAYS("This ButtonState should never happen");
        };
    }
}