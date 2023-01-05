#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Input
{
  public:
    /**
     * Return if the key was just pressed this frame
     */
    static bool IsKeyPressed(int key);

    /**
     * Return if the key is being held (was pressed the previous frame and not
     * yet released)
     */
    static bool IsKeyDown(int key);

    /**
     * Return the mouse's current position
     */
    static glm::vec2 GetMousePos();

    /**
     * Return if the mouse button was just pressed this frame
     */
    static bool IsMouseButtonPressed(int button);

    /**
     * Return if the mouse button was just released this frame
     */
    static bool IsMouseButtonReleased(int button);

    /**
     * Return if the mouse button is being held (was pressed the previous frame
     * and not yet released)
     */
    static bool IsMouseButtonDown(int button);

    /**
     * Callback from GLFW to track key events
     */
    static void OnKeyEvent(int key, int scancode, int action, int mods);

    /**
     * Callback from GLFW to track mouse position
     */
    static void OnCursorMove(float x_pos, float y_pos);

    /**
     * Callback from GLFW to track mouse button events
     */
    static void OnMouseButtonEvent(int button, int action, int mods);

    /**
     * Callback at the start of every frame
     */
    static void Update();
};