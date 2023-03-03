#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "engine/core/debug/Assert.h"
#include "engine/core/event/EventBus.h"
#include "engine/core/gfx/Window.h"
#include "engine/service/Service.h"

class InputService final : public Service
{
  public:
    /**
     * Return if the key was just pressed this frame
     */
    bool IsKeyPressed(int key);

    /**
     * Return if the key is being held (was pressed the previous frame and not
     * yet released)
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
};