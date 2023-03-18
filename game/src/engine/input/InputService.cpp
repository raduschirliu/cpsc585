#include "engine/input/InputService.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <array>
#include <string>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/input/Gamepad.h"
#include "engine/input/InputState.h"

using glm::ivec2;
using glm::vec2;
using std::array;
using std::string;
using std::vector;

struct InputEvent
{
    bool is_keyboard;
    int action;
    int key;

    InputEvent(bool is_keyboard, int action, int key)
        : is_keyboard(is_keyboard),
          action(action),
          key(key)
    {
    }
};

// Keep track of all gamepads
static vector<Gamepad> kGamepads;

// Keeps track of key events to be processed on next update
static vector<InputEvent> kInputEventQueue;

// Holds state of all keys: 1 == pressed, 0 == not pressed
static array<ButtonState, GLFW_KEY_LAST> kButtonStateMap;

// Keep track of mouse state (pos, button presses)
static MouseState kMouseState;

bool InputService::IsKeyPressed(int key)
{
    ASSERT_MSG(key >= 0 && key < kButtonStateMap.size(),
               "Invalid key code requested");
    return kButtonStateMap[key] == ButtonState::kPressed;
}

bool InputService::IsKeyDown(int key)
{
    ASSERT_MSG(key >= 0 && key < kButtonStateMap.size(),
               "Invalid key code requested");

    ButtonState state = kButtonStateMap[key];
    return state == ButtonState::kPressed || state == ButtonState::kDown;
}

ivec2 InputService::GetMousePos()
{
    return kMouseState.pos;
}

bool InputService::IsMouseButtonPressed(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == ButtonState::kPressed;
}

bool InputService::IsMouseButtonReleased(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == ButtonState::kReleased;
}

bool InputService::IsMouseButtonDown(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");

    ButtonState state = kMouseState.button_states[button];
    return state == ButtonState::kPressed || state == ButtonState::kDown;
}

void InputService::OnKeyEvent(int key, int scancode, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    // Ignore any other keys
    if (key >= 0 && key < kButtonStateMap.size())
    {
        kInputEventQueue.push_back(InputEvent(true, action, key));
    }
}

void InputService::OnCursorMove(double x_pos, double y_pos)
{
    kMouseState.SetPos(static_cast<int>(x_pos), static_cast<int>(y_pos));
}

void InputService::OnMouseButtonEvent(int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // Ignore any other mouse buttons
    if (button >= 0 && button < kMouseState.button_states.size())
    {
        kInputEventQueue.push_back(InputEvent(false, action, button));
    }
}

void InputService::OnJoystickChangedEvent(int joystick_id, int event)
{
    ASSERT_MSG(joystick_id < Gamepad::kGamepadCount,
               "Joystick ID must be valid");

    if (event == GLFW_CONNECTED)
    {
        debug::LogInfo("Joystick connected: {}", joystick_id);

        if (!kGamepads[joystick_id].Connect())
        {
            debug::LogInfo(
                "Connected joystick that is NOT a gamepad - ignoring");
        }
    }
    else if (event == GLFW_DISCONNECTED)
    {
        debug::LogInfo("Joystick disconnected: {}", joystick_id);
        kGamepads[joystick_id].Disconnect();
    }
    else
    {
        ASSERT_ALWAYS("This should never happen");
    }
}

bool InputService::IsGamepadActive(size_t gamepad_id)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return false;
    }

    return kGamepads[gamepad_id].GetActive();
}

float InputService::GetGamepadAxis(size_t gamepad_id, int axis)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return 0.0f;
    }

    return kGamepads[gamepad_id].GetAxis(axis);
}

bool InputService::IsGamepadButtonDown(size_t gamepad_id, int button)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return false;
    }

    return kGamepads[gamepad_id].IsButtonDown(button);
}

bool InputService::IsGamepadButtonPressed(size_t gamepad_id, int button)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return false;
    }

    return kGamepads[gamepad_id].IsButtonPressed(button);
}

void InputService::OnInit()
{
    GetEventBus().Subscribe<OnGuiEvent>(this);

    // Check if any gamepads are connected
    for (int i = 0; i < static_cast<int>(Gamepad::kGamepadCount); i++)
    {
        Gamepad gamepad(i);

        if (gamepad.Connect())
        {
            debug::LogInfo("Found controller, ID: {}", i);
        }

        kGamepads.push_back(gamepad);
    }
}

void InputService::OnCleanup()
{
    kGamepads.clear();
}

std::string_view InputService::GetName() const
{
    return "InputService";
}

void InputService::OnUpdate()
{
    // Update keys in keyboard state map
    for (auto& state : kButtonStateMap)
    {
        if (state == ButtonState::kPressed)
        {
            state = ButtonState::kDown;
        }
        else if (state == ButtonState::kReleased)
        {
            state = ButtonState::kUp;
        }
    }

    // Update mouse button states
    for (auto& state : kMouseState.button_states)
    {
        if (state == ButtonState::kPressed)
        {
            state = ButtonState::kDown;
        }
        else if (state == ButtonState::kReleased)
        {
            state = ButtonState::kUp;
        }
    }

    // Process queue and add to state map
    while (!kInputEventQueue.empty())
    {
        auto& event = kInputEventQueue.front();

        if (event.is_keyboard)
        {
            // Keyboard key events
            if (event.action == GLFW_PRESS)
            {
                kButtonStateMap[event.key] = ButtonState::kPressed;
            }
            else if (event.action == GLFW_RELEASE)
            {
                kButtonStateMap[event.key] = ButtonState::kReleased;
            }
        }
        else
        {
            // Mouse button events
            if (event.action == GLFW_PRESS)
            {
                kMouseState.button_states[event.key] = ButtonState::kPressed;
            }
            else if (event.action == GLFW_RELEASE)
            {
                kMouseState.button_states[event.key] = ButtonState::kReleased;
            }
        }

        kInputEventQueue.erase(kInputEventQueue.begin());
    }

    UpdateGamepads();

    // Check for debug menu
    if (IsKeyPressed(GLFW_KEY_F4))
    {
        show_debug_menu_ = !show_debug_menu_;
    }
}

void InputService::OnGui()
{
    if (!show_debug_menu_)
    {
        return;
    }

    if (!ImGui::Begin("InputService Debug", &show_debug_menu_))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Gamepad axis threshold: %f", Gamepad::kAxisThreshold);
    ImGui::Text("Gamepads:");
    ImGui::Spacing();

    for (auto& gamepad : kGamepads)
    {
        const string name_label = fmt::format("Name: {}", gamepad.GetName());

        ImGui::PushID(gamepad.GetId());
        if (ImGui::CollapsingHeader(name_label.c_str()))
        {
            ImGui::Text("Joystick ID: %d", gamepad.GetId());

            ImGui::Text("Axes:");
            ImGui::Indent(5.0f);
            for (int i = 0; i < Gamepad::kAxisCount; i++)
            {
                ImGui::Text("%s: %f", Gamepad::GetAxisName(i).c_str(),
                            gamepad.GetAxis(i));
            }
            ImGui::Unindent(5.0f);
        }
        ImGui::PopID();
    }

    ImGui::End();
}

void InputService::UpdateGamepads()
{
    for (auto& gamepad : kGamepads)
    {
        gamepad.UpdateState();
    }
}
