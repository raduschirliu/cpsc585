#include "engine/gfx/Window.h"

#include <iostream>

#include "engine/gfx/Log.h"

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
    : handle_(nullptr),
      callbacks_(callbacks)
{
    // specify OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // needed for mac?
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // create window
    handle_ = std::unique_ptr<GLFWwindow, WindowDeleter>(
        glfwCreateWindow(width, height, title, monitor, share));
    if (!handle_)
    {
        Log::error("WINDOW failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window.");
    }
    glfwMakeContextCurrent(handle_.get());

    // initialize OpenGL extensions for the current context (this window)
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        Log::error("Window glewInit error: {}", glewGetErrorString(err));
        throw std::runtime_error("Failed to initialize GLEW");
    }

    glfwSetWindowSizeCallback(handle_.get(), defaultWindowSizeCallback);
    ConnectCallbacks();
}

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor,
               GLFWwindow* share)
    : Window(nullptr, width, height, title, monitor, share)
{
}

void Window::ConnectCallbacks()
{
    // bind meta callbacks to actual callbacks
    glfwSetKeyCallback(handle_.get(), keyMetaCallback);
    glfwSetMouseButtonCallback(handle_.get(), mouseButtonMetaCallback);
    glfwSetCursorPosCallback(handle_.get(), cursorPosMetaCallback);
    glfwSetScrollCallback(handle_.get(), scrollMetaCallback);
    glfwSetWindowSizeCallback(handle_.get(), windowSizeMetaCallback);
}

void Window::SetCallbacks(std::shared_ptr<IWindowEventListener> callbacks_)
{
    // set userdata of window to point to the object that carries out the
    // callbacks
    callbacks_ = callbacks_;
    glfwSetWindowUserPointer(handle_.get(), callbacks_.get());
}

glm::ivec2 Window::GetPos() const
{
    int x, y;
    glfwGetWindowPos(handle_.get(), &x, &y);
    return glm::ivec2(x, y);
}

glm::ivec2 Window::GetSize() const
{
    int w, h;
    glfwGetWindowSize(handle_.get(), &w, &h);
    return glm::ivec2(w, h);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(handle_.get()) != 0;
}

void Window::MakeContextCurrent()
{
    glfwMakeContextCurrent(handle_.get());
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(handle_.get());
}

GLFWwindow* Window::GetWindowHandle() const
{
    return handle_.get();
}