#include "engine/App.h"

using std::make_unique;
using std::string_view;

App::App()
    : running_(false),
      window_(),
      renderer_(GetWindow()),
      scenes_{},
      service_provider_()
{
}

void App::Start()
{
    window_.Create(100, 100, "app");
    window_.SetCallbacks(shared_from_this());

    renderer_.Init();
    Init();

    Run();

    Cleanup();
    renderer_.Cleanup();
}

void App::Init()
{
    // To be overridden if needed
}

void App::Cleanup()
{
    // To be overridden if needed
}

Scene& App::AddScene(string_view name)
{
    auto scene = make_unique<Scene>(name, service_provider_);
    scenes_.push_back(std::move(scene));

    return *scenes_.back();
}

Window& App::GetWindow()
{
    return window_;
}

void App::Run()
{
    running_ = true;

    while (running_ && !window_.ShouldClose())
    {
        window_.PollEvents();

        // TODO: Use fixed timestep
        // TODO: Update physics

        // TODO: Prepare objects to be rendered
        renderer_.RenderFrame();

        window_.SwapBuffers();
    }
}

void App::OnKeyEvent(int key, int scancode, int action, int mods)
{
}

void App::OnMouseButtonEvent(int button, int action, int mods)
{
}

void App::OnCursorMove(double xpos, double ypos)
{
}

void App::OnScroll(double xoffset, double yoffset)
{
}

void App::OnWindowSizeChanged(int width, int height)
{
}
