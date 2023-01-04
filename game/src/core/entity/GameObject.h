#pragma once

#include <memory>
#include <vector>

#include "core/ShaderManager.h"
#include "core/entity/Transform.h"
#include "core/gfx/Geometry.h"
#include "core/gfx/ShaderProgram.h"
#include "core/gfx/Texture.h"
#include "core/material/Material.h"

class GameObject
{
  public:
    // No copy allowed since the handles in Geometry aren't copyable
    GameObject(const GameObject& other) = delete;
    GameObject& operator=(const GameObject& other) = delete;
    // Moving is okay though :-)
    GameObject(GameObject&& other) = default;
    GameObject& operator=(GameObject&& other) = default;

    GameObject();

    virtual void DrawGui();
    virtual void Draw();
    virtual void Update(float delta_time);

    void SetActive(bool state);
    bool SetupShader();
    void SetGeometry(std::shared_ptr<Geometry> geometry);
    void SetTexture(std::shared_ptr<Texture> texture);
    void SetMaterial(std::shared_ptr<IMaterial> material);

    void AddChild(std::shared_ptr<GameObject> object);

    bool Active() const;
    std::shared_ptr<::Transform> Transform();
    std::shared_ptr<::Geometry> Geometry();

  protected:
    virtual glm::mat4 BuildModelMatrix();

  private:
    bool active_;
    std::shared_ptr<::Transform> transform_;
    std::shared_ptr<::Geometry> geometry_;
    std::shared_ptr<Texture> texture_;
    std::shared_ptr<IMaterial> material_;
    std::vector<std::shared_ptr<GameObject>> children_;
};