#include "engine/core/gfx/Window.h"

#include <iostream>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/gfx/GLDebug.h"

using glm::ivec2;
using std::shared_ptr;
using std::string;

static Window* kWindowInstance = nullptr;

// ---------------------------
// static function definitions
// ---------------------------

void Window::KeyMetaCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnKeyEvent(key, scancode, action, mods);
    }
}

void Window::MouseButtonMetaCallback(GLFWwindow* window, int button, int action,
                                     int mods)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnMouseButtonEvent(button, action, mods);
    }
}

void Window::CursorPosMetaCallback(GLFWwindow* window, double xpos, double ypos)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnCursorMove(xpos, ypos);
    }
}

void Window::ScrollMetaCallback(GLFWwindow* window, double xoffset,
                                double yoffset)
{
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnScroll(xoffset, yoffset);
    }
}

void Window::WindowSizeMetaCallback(GLFWwindow* window, int width, int height)
{
    kWindowInstance->size_ = ivec2(width, height);

    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(window));
    if (callbacks)
    {
        callbacks->OnWindowSizeChanged(width, height);
    }
}

void Window::JoystickChangedMetaCallback(int joystick_id, int event)
{
    if (!kWindowInstance)
    {
        return;
    }

    GLFWwindow* handle = kWindowInstance->GetWindowHandle();
    IWindowEventListener* callbacks =
        static_cast<IWindowEventListener*>(glfwGetWindowUserPointer(handle));
    if (callbacks)
    {
        callbacks->OnJoystickChangedEvent(joystick_id, event);
    }
}

// ----------------------
// non-static definitions
// ----------------------

Window::Window() : handle_(nullptr), callbacks_(nullptr)
{
    ASSERT_MSG(!kWindowInstance, "Cannot have multiple windows");
}

void Window::Create(int width, int height, const char* title)
{
    ASSERT_MSG(!handle_, "Window has already been created");

    // Use OpenGL 410 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // needed for mac?
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // 4x MSAA
    // glfwWindowHint(GLFW_SAMPLES, 4);

    // create window
    handle_ = std::unique_ptr<GLFWwindow, WindowDeleter>(
        glfwCreateWindow(width, height, title, nullptr, nullptr));
    if (!handle_)
    {
        throw std::runtime_error("Failed to create GLFW window");
    }
    MakeContextCurrent();

    // initialize OpenGL extensions for the current context (this window)
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        debug::LogError("Window glewInit error: {}", glewGetErrorString(err));
        throw std::runtime_error("Failed to initialize GLEW");
    }

    ConnectCallbacks();
    debug::LogInfo("Window created successfully");

#ifndef NDEBUG
    GLDebug::enable();
#endif
}

void Window::ConnectCallbacks()
{
    // bind meta callbacks to actual callbacks
    glfwSetKeyCallback(handle_.get(), KeyMetaCallback);
    glfwSetMouseButtonCallback(handle_.get(), MouseButtonMetaCallback);
    glfwSetCursorPosCallback(handle_.get(), CursorPosMetaCallback);
    glfwSetScrollCallback(handle_.get(), ScrollMetaCallback);
    glfwSetWindowSizeCallback(handle_.get(), WindowSizeMetaCallback);
    glfwSetJoystickCallback(JoystickChangedMetaCallback);
}

void Window::SetCallbacks(shared_ptr<IWindowEventListener> callbacks)
{
    callbacks_ = callbacks;
    kWindowInstance = this;
    glfwSetWindowUserPointer(handle_.get(), callbacks_.get());
}

void Window::SetSize(const glm::ivec2& size)
{
    ASSERT_MSG(handle_, "Window must exist");
    glfwSetWindowSize(handle_.get(), size.x, size.y);
}

void Window::SetTitle(const string& title)
{
    glfwSetWindowTitle(handle_.get(), title.c_str());
}

void Window::PollEvents()
{
    glfwPollEvents();
}

glm::ivec2 Window::GetPos() const
{
    int x, y;
    glfwGetWindowPos(handle_.get(), &x, &y);
    return glm::ivec2(x, y);
}

const glm::ivec2& Window::GetSize() const
{
    return size_;
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