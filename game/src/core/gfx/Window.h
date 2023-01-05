#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <memory>

// Class that specifies the interface for the most common GLFW callbacks
//
// These are the default implementations. You can write your own class that
// extends this one and overrides the implementations with your own
class IWindowEventListener
{
  public:
    virtual void OnKeyEvent(int key, int scancode, int action, int mods)
    {
    }

    virtual void OnMouseButtonEvent(int button, int action, int mods)
    {
    }

    virtual void OnCursorMove(double xpos, double ypos)
    {
    }

    virtual void OnScroll(double xoffset, double yoffset)
    {
    }

    virtual void OnWindowSizeChanged(int width, int height)
    {
    }
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
    Window(std::shared_ptr<IWindowEventListener> callbacks, int width,
           int height, const char* title, GLFWmonitor* monitor = NULL,
           GLFWwindow* share = NULL);
    Window(int width, int height, const char* title,
           GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);

    bool ShouldClose();
    void MakeContextCurrent();
    void SwapBuffers();

    void SetCallbacks(std::shared_ptr<IWindowEventListener> callbacks_);

    glm::ivec2 GetPos() const;
    glm::ivec2 GetSize() const;
    GLFWwindow* GetWindowHandle() const;

  private:
    // owning ptr (from GLFW)
    std::unique_ptr<GLFWwindow, WindowDeleter> handle_;
    // optional shared owning ptr (user provided)
    std::shared_ptr<IWindowEventListener> callbacks_;

    void ConnectCallbacks();

    static void defaultWindowSizeCallback(GLFWwindow* window, int width,
                                          int height)
    {
        glViewport(0, 0, width, height);
    }

    // Meta callback functions. These bind to the actual glfw callback,
    // get the actual callback method from user data, and then call that.
    static void keyMetaCallback(GLFWwindow* window, int key, int scancode,
                                int action, int mods);
    static void mouseButtonMetaCallback(GLFWwindow* window, int button,
                                        int action, int mods);
    static void cursorPosMetaCallback(GLFWwindow* window, double xpos,
                                      double ypos);
    static void scrollMetaCallback(GLFWwindow* window, double xoffset,
                                   double yoffset);
    static void windowSizeMetaCallback(GLFWwindow* window, int width,
                                       int height);
};
