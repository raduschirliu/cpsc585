#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/gfx/Window.h"
#include "engine/gui/OnGuiEvent.h"
#include "engine/service/Service.h"

class InputService final : public Service, public IEventSubscriber<OnGuiEvent>
{
  public:
    /**
     * Return if the key was just pressed. Will return true for only one frame
     */
    bool IsKeyPressed(int key);

    /**
     * Return if the key is being held down (was pressed the previous frame and
     * not yet released). Will return true as long as the button is depressed
     */
    bool IsKeyDown(int key);

    /**
     * Return the mouse's current position
     */
    glm::ivec2 GetMousePos();

    /**
     * Return if the mouse button was just pressed this frame
     */
    bool IsMouseButtonPressed(int button);

    /**
     * Return if the mouse button was just released this frame
     */
    bool IsMouseButtonReleased(int button);

    /**
     * Return if the mouse button is being held (was pressed the previous frame
     * and not yet released)
     */
    bool IsMouseButtonDown(int button);

    /**
     * Return true if a gamepad with given ID is connected & initialized, false
     * otherwise
     */
    bool IsGamepadActive(size_t gamepad_id);

    /**
     * Return a value in the range [-1.0f, 1.0f] representing a gamepad axis. If
     * the gamepad doesn't exist, return 0.0f
     */
    float GetGamepadAxis(size_t gamepad_id, int axis);

    /**
     * Return if the gamepad button is being held down (was pressed the previous
     * frame and not yet released). Will return true as long as the button is
     * depressed.
     *
     * If the gamepad doesn't exist, will return false.
     */
    bool IsGamepadButtonDown(size_t gamepad_id, int button);

    /**
     * Return if the gamepad button was just pressed. Will return true for only
     * one frame.
     *
     * If the gamepad doesn't exist, will return false.
     */
    bool IsGamepadButtonPressed(size_t gamepad_id, int button);

    /**
     * Callback from GLFW to track key events
     */
    static void OnKeyEvent(int key, int scancode, int action, int mods);

    /**
     * Callback from GLFW to track mouse position
     */
    static void OnCursorMove(double x_pos, double y_pos);

    /**
     * Callback from GLFW to track mouse button events
     */
    static void OnMouseButtonEvent(int button, int action, int mods);

    /**
     * Callback from GLFW to track when a joystick connects/disconnects
     */
    static void OnJoystickChangedEvent(int joystick_id, int event);

    // From Service
    void OnInit() override;
    void OnUpdate() override;
    void OnCleanup() override;
    std::string_view GetName() const override;

    // From Event subscribers
    void OnGui() override;

  private:
    bool show_debug_menu_ = false;

    void UpdateGamepads();
};