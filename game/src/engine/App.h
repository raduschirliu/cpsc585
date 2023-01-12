#pragma once

#include <memory>
#include <string_view>

#include "engine/core/gfx/Window.h"
#include "engine/render/Renderer.h"
#include "engine/scene/Scene.h"
#include "engine/service/ServiceProvider.h"

class App : public std::enable_shared_from_this<App>,
            public IWindowEventListener
{
  public:
    App();

    void Start();

    // From IWindowEventListener
    void OnKeyEvent(int key, int scancode, int action, int mods) override;
    void OnMouseButtonEvent(int button, int action, int mods) override;
    void OnCursorMove(double xpos, double ypos) override;
    void OnScroll(double xoffset, double yoffset) override;
    void OnWindowSizeChanged(int width, int height) override;

  protected:
    virtual void Init();
    virtual void Cleanup();

    template <class ServiceType>
        requires std::derived_from<ServiceType, IService>
    void AddService()
    {
        auto service = std::make_unique<ServiceType>();
        service->Init();
        service_provider_.AddService(std::move(service));
    }

    Scene& AddScene(std::string_view name);
    Window& GetWindow();

  private:
    bool running_;
    Window window_;
    Renderer renderer_;
    std::vector<std::unique_ptr<Scene>> scenes_;
    ServiceProvider service_provider_;

    void Run();
};