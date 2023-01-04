#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <type_traits>

#include "core/ShaderManager.h"
#include "core/Utils.h"
#include "core/entity/Camera.h"
#include "core/entity/GameObject.h"
#include "core/scene/Listeners.h"

class Scene : public IKeyListener,
              public IMouseListener,
              public IWindowSizeListener
{
  public:
    Scene(std::unique_ptr<Camera> camera);

    // Called before scene is loaded
    virtual void Init();

    // Called during render loop for GUI drawing
    virtual void RenderGui();

    // Called during game loop before rendering
    virtual void UpdateScene(float delta_time);

    // Called during game loop to render the scene
    virtual void RenderScene();

    void OnWindowSizeChanged(glm::ivec2 size) override;

    template <class T,
              std::enable_if_t<std::is_base_of_v<GameObject, T>, bool> = true>
    std::shared_ptr<T> AddGameObject(std::shared_ptr<T> object)
    {
        ASSERT_MSG(object, "Cannot add invalid GameObject to scene");
        game_objects_.push_back(object);
        return object;
    }

    template <class T>
    T& Camera()
    {
        ASSERT_MSG(camera_, "Must have a Camera in the scene");
        return static_cast<T&>(*camera_);
    }

  private:
    std::unique_ptr<::Camera> camera_;
    std::vector<std::shared_ptr<GameObject>> game_objects_;
};
