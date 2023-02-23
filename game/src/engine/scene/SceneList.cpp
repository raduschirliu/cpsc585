#include "engine/scene/SceneList.h"

using std::make_unique;
using std::string_view;
using std::unique_ptr;

SceneList::SceneList() : scenes_{}, active_scene_(nullptr)
{
}

Scene& SceneList::GetActiveScene()
{
    ASSERT_MSG(active_scene_, "Must have an active scene");
    return *active_scene_;
}

Scene& SceneList::AddScene(unique_ptr<Scene> scene)
{
    scenes_.push_back(std::move(scene));
    return *scenes_.back();
}

void SceneList::SetActiveScene(string_view name)
{
    for (auto& scene : scenes_)
    {
        if (scene->GetName() == name)
        {
            active_scene_ = scene.get();
            active_scene_->Load();
            return;
        }
    }

    ASSERT_MSG(false, "Scene with name does not exist");
}

bool SceneList::HasActiveScene() const
{
    return active_scene_ != nullptr;
}