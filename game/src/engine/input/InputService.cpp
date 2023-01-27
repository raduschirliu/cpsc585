#include "engine/input/InputService.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <array>
#include <vector>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"

using glm::ivec2;
using glm::vec2;
using std::array;
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

void InputService::OnInit()
{
    Log::info("InputService - Initialized");
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
}
