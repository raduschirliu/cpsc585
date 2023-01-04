#include "core/Input.h"

#include <GLFW/glfw3.h>

#include <array>
#include <vector>

#include "core/Utils.h"

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
    vec2 pos;
    array<KeyState, GLFW_MOUSE_BUTTON_LAST> button_states;

    MouseState() : pos(0.0f, 0.0f), button_states{}
    {
    }

    void SetPos(float x, float y)
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

bool Input::IsKeyPressed(int key)
{
    ASSERT_MSG(key >= 0 && key < kKeyStateMap.size(),
               "Invalid key code requested");
    return kKeyStateMap[key] == KeyState::kPressed;
}

bool Input::IsKeyDown(int key)
{
    ASSERT_MSG(key >= 0 && key < kKeyStateMap.size(),
               "Invalid key code requested");

    KeyState state = kKeyStateMap[key];
    return state == KeyState::kPressed || state == KeyState::kDown;
}

vec2 Input::GetMousePos()
{
    return kMouseState.pos;
}

bool Input::IsMouseButtonPressed(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == KeyState::kPressed;
}

bool Input::IsMouseButtonReleased(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");
    return kMouseState.button_states[button] == KeyState::kReleased;
}

bool Input::IsMouseButtonDown(int button)
{
    ASSERT_MSG(button >= 0 && button < kMouseState.button_states.size(),
               "Invalid mouse button");

    KeyState state = kMouseState.button_states[button];
    return state == KeyState::kPressed || state == KeyState::kDown;
}

void Input::OnKeyEvent(int key, int scancode, int action, int mods)
{
    // Ignore any other keys
    if (key >= 0 && key < kKeyStateMap.size())
    {
        kInputEventQueue.push_back(InputEvent(true, action, key));
    }
}

void Input::OnCursorMove(float x_pos, float y_pos)
{
    kMouseState.SetPos(x_pos, y_pos);
}

void Input::OnMouseButtonEvent(int button, int action, int mods)
{
    // Ignore any other mouse buttons
    if (button >= 0 && button < kMouseState.button_states.size())
    {
        kInputEventQueue.push_back(InputEvent(false, action, button));
    }
}

void Input::Update()
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