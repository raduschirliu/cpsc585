#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Class that specifies the interface for the most common GLFW callbacks
//
// These are the default implementations. You can write your own class that
// extends this one and overrides the implementations with your own
class IWindowEventListener
{
  public:
    virtual void OnKeyEvent(int key, int scancode, int action, int mods) = 0;
    virtual void OnMouseButtonEvent(int button, int action, int mods) = 0;
    virtual void OnCursorMove(double xpos, double ypos) = 0;
    virtual void OnScroll(double xoffset, double yoffset) = 0;
    virtual void OnWindowSizeChanged(int width, int height) = 0;
    virtual void OnJoystickChangedEvent(int joystick_id, int event) = 0;
};

// Functor for deleting a GLFW window.
//
// This is used as a custom deleter with std::unique_ptr so that the window
// is properly destroyed when std::unique_ptr needs to clean up its resource
struct WindowDeleter
{
    void operator()(GLFWwindow* window) const
    {
        glfwDestroyWindow(window);
    }
};

// Main class for creating and interacting with a GLFW window.
// Only wraps the most fundamental parts of the API
class Window
{
  public:
    Window();

    void Create(int width, int height, const char* title);
    bool ShouldClose();
    void MakeContextCurrent();
    void SwapBuffers();
    void PollEvents();

    void SetCallbacks(std::shared_ptr<IWindowEventListener> callbacks);
    void SetSize(const glm::ivec2& size);
    void SetTitle(const std::string& title);

    glm::ivec2 GetPos() const;
    glm::ivec2 GetSize() const;
    GLFWwindow* GetWindowHandle() const;

  private:
    // owning ptr (from GLFW)
    std::unique_ptr<GLFWwindow, WindowDeleter> handle_;
    std::shared_ptr<IWindowEventListener> callbacks_;

    void ConnectCallbacks();

    // Meta callback functions. These bind to the actual glfw callback,
    // get the actual callback method from user data, and then call that.
    static void KeyMetaCallback(GLFWwindow* window, int key, int scancode,
                                int action, int mods);
    static void MouseButtonMetaCallback(GLFWwindow* window, int button,
                                        int action, int mods);
    static void CursorPosMetaCallback(GLFWwindow* window, double xpos,
                                      double ypos);
    static void ScrollMetaCallback(GLFWwindow* window, double xoffset,
                                   double yoffset);
    static void WindowSizeMetaCallback(GLFWwindow* window, int width,
                                       int height);
    static void JoystickChangedMetaCallback(int joystick_id, int event);
};
