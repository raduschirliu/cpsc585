#include "core/gfx/Window.h"

#include <iostream>

#include "core/gfx/Log.h"

// ---------------------------
// static function definitions
// ---------------------------

void Window::keyMetaCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnKeyEvent(key, scancode, action, mods);
    }
}

void Window::mouseButtonMetaCallback(GLFWwindow* window, int button, int action,
                                     int mods)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnMouseButtonEvent(button, action, mods);
    }
}

void Window::cursorPosMetaCallback(GLFWwindow* window, double xpos, double ypos)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnCursorMove(xpos, ypos);
    }
}

void Window::scrollMetaCallback(GLFWwindow* window, double xoffset,
                                double yoffset)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnScroll(xoffset, yoffset);
    }
}

void Window::windowSizeMetaCallback(GLFWwindow* window, int width, int height)
{
    defaultWindowSizeCallback(window, width, height);

    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnWindowSizeChanged(width, height);
    }
}

// ----------------------
// non-static definitions
// ----------------------

Window::Window(std::shared_ptr<IWindowEventListener> callbacks, int width,
               int height, const char* title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : window(nullptr),
      callbacks(callbacks)
{
    // specify OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // needed for mac?
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // create window
    window = std::unique_ptr<GLFWwindow, WindowDeleter>(
        glfwCreateWindow(width, height, title, monitor, share));
    if (!window)
    {
        Log::error("WINDOW failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window.");
    }
    glfwMakeContextCurrent(window.get());

    // initialize OpenGL extensions for the current context (this window)
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        Log::error("WINDOW glewInit error:{}", glewGetErrorString(err));
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glfwSetWindowSizeCallback(window.get(), defaultWindowSizeCallback);
    connectCallbacks();
}

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : Window(nullptr, width, height, title, monitor, share)
{
}

void Window::connectCallbacks()
{
    // bind meta callbacks to actual callbacks
    glfwSetKeyCallback(window.get(), keyMetaCallback);
    glfwSetMouseButtonCallback(window.get(), mouseButtonMetaCallback);
    glfwSetCursorPosCallback(window.get(), cursorPosMetaCallback);
    glfwSetScrollCallback(window.get(), scrollMetaCallback);
    glfwSetWindowSizeCallback(window.get(), windowSizeMetaCallback);
}

void Window::setCallbacks(std::shared_ptr<IWindowEventListener> callbacks_)
{
    // set userdata of window to point to the object that carries out the
    // callbacks
    callbacks = callbacks_;
    glfwSetWindowUserPointer(window.get(), callbacks.get());
}

glm::ivec2 Window::getPos() const
{
    int x, y;
    glfwGetWindowPos(window.get(), &x, &y);
    return glm::ivec2(x, y);
}

glm::ivec2 Window::getSize() const
{
    int w, h;
    glfwGetWindowSize(window.get(), &w, &h);
    return glm::ivec2(w, h);
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window.get()) != 0;
}

void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(window.get());
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window.get());
}
