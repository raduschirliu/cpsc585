#include "engine/scene/Scene.h"

using std::move;
using std::shared_ptr;
using std::unique_ptr;

Scene::Scene(unique_ptr<::Camera> camera) : camera_(move(camera))
{
}

void Scene::RenderGui()
{
    if (camera_)
    {
        camera_->RenderGui();
    }

    for (auto& obj : game_objects_)
    {
        obj->DrawGui();
    }
}

void Scene::UpdateScene(float delta_time)
{
    if (camera_)
    {
        camera_->Update(delta_time);
    }

    for (auto& obj : game_objects_)
    {
        obj->Update(delta_time);
    }
}

void Scene::Init()
{
    if (camera_)
    {
        camera_->SetViewportSize(GetWindowSize());
    }
}

void Scene::RenderScene()
{
    if (camera_)
    {
        ASSERT_MSG(camera_, "Must have a camera in the scene");
        camera_->Render();
    }

    for (auto& obj : game_objects_)
    {
        obj->Draw();
    }
}

void Scene::OnWindowSizeChanged(glm::ivec2 size)
{
    if (camera_)
    {
        camera_->SetViewportSize(size);
    }
}