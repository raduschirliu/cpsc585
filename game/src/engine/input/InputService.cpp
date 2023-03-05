#include "engine/input/InputService.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <array>
#include <string>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

using glm::ivec2;
using glm::vec2;
using std::array;
using std::string;
using std::vector;

enum class KeyState : uint8_t
{
    kReleased = 0,
    kUp,
    kPressed,
    kDown
};

struct MouseState
{
    ivec2 pos;
    array<KeyState, GLFW_MOUSE_BUTTON_LAST> button_states;

    MouseState() : pos(0, 0), button_states{}
    {
    }

    void SetPos(int x, int y)
    {
        pos.x = x;
        pos.y = y;
    }
};

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

struct Gamepad
{
    int id;
    bool active;
    string name;
    GLFWgamepadstate state;
};

// Gamepad axis values will be ignored if abs(val) < threshold
static constexpr float kGamepadAxisThreshold = 0.1f;

static constexpr array<int, 4> kGamepadThresholdAxes = {
    GLFW_GAMEPAD_AXIS_LEFT_X,
    GLFW_GAMEPAD_AXIS_LEFT_Y,
    GLFW_GAMEPAD_AXIS_RIGHT_X,
    GLFW_GAMEPAD_AXIS_RIGHT_Y,
};

// Gamepad axis names
static const array<string, GLFW_GAMEPAD_AXIS_LAST + 1> kGamepadAxisNames = {
    "LeftX", "LeftY", "RightX", "RightY", "LeftTrigger", "RightTrigger"};

// Keep track of all gamepads
static vector<Gamepad> kGamepads(GLFW_JOYSTICK_LAST + 1);

// Keeps track of key events to be processed on next update
static vector<InputEvent> kInputEventQueue;

// Holds state of all keys: 1 == pressed, 0 == not pressed
static array<KeyState, GLFW_KEY_LAST> kKeyStateMap;

// Keep track of mouse state (pos, button presses)
static MouseState kMouseState;

bool InputService::IsKeyPressed(int key)
{
    ASSERT_MSG(key >= 0 && key < kKeyStateMap.size(),
               "Invalid key code requested");
    return kKeyStateMap[key] == KeyState::kPressed;
}

bool InputService::IsKeyDown(int key)
{
    ASSERT_MSG(key >= 0 && key < kKeyStateMap.size(),
               "Invalid key code requested");

    KeyState state = kKeyStateMap[key];
    return state == KeyState::kPressed || state == KeyState::kDown;
}

ivec2 InputService::GetMousePos()
{
    return kMouseState.pos;
}

bool InputService::IsMouseButtonPressed(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == KeyState::kPressed;
}

bool InputService::IsMouseButtonReleased(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == KeyState::kReleased;
}

bool InputService::IsMouseButtonDown(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");

    KeyState state = kMouseState.button_states[button];
    return state == KeyState::kPressed || state == KeyState::kDown;
}

void InputService::OnKeyEvent(int key, int scancode, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    // Ignore any other keys
    if (key >= 0 && key < kKeyStateMap.size())
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
    if (event == GLFW_CONNECTED)
    {
        Log::info("Joystick connected: {}", joystick_id);

        if (!TryRegisterController(joystick_id))
        {
            Log::info("Connected joystick that is NOT a gamepad - ignoring");
        }
    }
    else if (event == GLFW_DISCONNECTED)
    {
        Log::info("Joystick disconnected: {}", joystick_id);
        kGamepads[joystick_id].active = false;
        kGamepads[joystick_id].name = "INACTIVE";
    }
    else
    {
        ASSERT_ALWAYS("This should never happen");
    }
}

float InputService::GetGamepadAxis(size_t gamepad_id, int axis)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return 0.0f;
    }

    if (axis >= kGamepadAxisNames.size() || axis < 0)
    {
        ASSERT_ALWAYS("Invalid gamepad axis");
        return 0.0f;
    }

    const Gamepad& gamepad = kGamepads[gamepad_id];

    if (!gamepad.active)
    {
        return 0.0f;
    }

    return gamepad.state.axes[axis];
}

bool InputService::IsGamepadButtonDown(size_t gamepad_id, int button)
{
    if (gamepad_id >= kGamepads.size())
    {
        ASSERT_ALWAYS("Invalid gamepad ID");
        return false;
    }

    if (button < 0 || button > GLFW_GAMEPAD_BUTTON_LAST)
    {
        ASSERT_ALWAYS("Invalid gamepad button");
        return false;
    }

    const Gamepad& gamepad = kGamepads[gamepad_id];

    if (!gamepad.active)
    {
        return false;
    }

    return gamepad.state.buttons[button] == GLFW_PRESS;
}

void InputService::OnInit()
{
    GetEventBus().Subscribe<OnGuiEvent>(this);

    // Check if any gamepads are connected
    for (int i = 0; i < kGamepads.size(); i++)
    {
        kGamepads[i].id = i;
        kGamepads[i].active = false;
        kGamepads[i].name = "INACTIVE";

        if (TryRegisterController(i))
        {
            Log::info("Found controller, ID: {}", i);
        }
    }
}

void InputService::OnCleanup()
{
}

std::string_view InputService::GetName() const
{
    return "InputService";
}

void InputService::OnUpdate()
{
    // Update keys in keyboard state map
    for (auto& state : kKeyStateMap)
    {
        if (state == KeyState::kPressed)
        {
            state = KeyState::kDown;
        }
        else if (state == KeyState::kReleased)
        {
            state = KeyState::kUp;
        }
    }

    // Update mouse button states
    for (auto& state : kMouseState.button_states)
    {
        if (state == KeyState::kPressed)
        {
            state = KeyState::kDown;
        }
        else if (state == KeyState::kReleased)
        {
            state = KeyState::kUp;
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
                kKeyStateMap[event.key] = KeyState::kPressed;
            }
            else if (event.action == GLFW_RELEASE)
            {
                kKeyStateMap[event.key] = KeyState::kReleased;
            }
        }
        else
        {
            // Mouse button events
            if (event.action == GLFW_PRESS)
            {
                kMouseState.button_states[event.key] = KeyState::kPressed;
            }
            else if (event.action == GLFW_RELEASE)
            {
                kMouseState.button_states[event.key] = KeyState::kReleased;
            }
        }

        kInputEventQueue.erase(kInputEventQueue.begin());
    }

    // Update gamepads
    for (auto& controller : kGamepads)
    {
        glfwGetGamepadState(controller.id, &controller.state);

        for (auto axis_idx : kGamepadThresholdAxes)
        {
            auto& axis_val = controller.state.axes[axis_idx];
            if (abs(axis_val) < kGamepadAxisThreshold)
            {
                axis_val = 0.0f;
            }
        }
    }

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

    ImGui::Text("Gamepad axis threshold: %f", kGamepadAxisThreshold);
    ImGui::Text("Gamepads:");
    ImGui::Spacing();

    for (auto& gamepad : kGamepads)
    {
        const string name_label = fmt::format("Name: {}", gamepad.name);

        if (ImGui::CollapsingHeader(name_label.c_str()))
        {
            ImGui::Text("Joystick ID: %d", gamepad.id);

            ImGui::Text("Axes:");
            ImGui::Indent(5.0f);
            for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; i++)
            {
                ImGui::Text("%s: %f", kGamepadAxisNames[i].c_str(),
                            gamepad.state.axes[i]);
            }
            ImGui::Unindent(5.0f);
        }
        ImGui::PopID();
    }

    ImGui::End();
}

bool InputService::TryRegisterController(int id)
{
    if (id < 0 || id >= kGamepads.size())
    {
        return false;
    }

    if (!glfwJoystickIsGamepad(id))
    {
        return false;
    }

    Gamepad& gamepad = kGamepads[id];
    gamepad.active = true;
    gamepad.id = id;
    gamepad.name = glfwGetGamepadName(id);

    return true;
}