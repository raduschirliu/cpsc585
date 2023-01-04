#include "core/entity/GameObject.h"

#include "core/Utils.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::vector;

GameObject::GameObject()
    : active_(true),
      transform_(make_shared<::Transform>()),
      geometry_(nullptr),
      texture_(nullptr),
      material_(make_shared<BasicMaterial>()),
      children_{}
{
}

void GameObject::SetActive(bool state)
{
    active_ = state;
}

bool GameObject::SetupShader()
{
    if (transform_ && material_)
    {
        material_->Apply(BuildModelMatrix());
        return true;
    }

    return false;
}

void GameObject::DrawGui()
{
    for (auto& child : children_)
    {
        child->DrawGui();
    }
}

void GameObject::Draw()
{
    if (active_ && transform_)
    {
        if (SetupShader() && geometry_)
        {
            geometry_->Draw();
        }

        for (auto& child : children_)
        {
            child->Draw();
        }
    }
}

void GameObject::Update(float delta_time)
{
    if (material_)
    {
        material_->Update(delta_time);
    }

    for (auto& child : children_)
    {
        child->Update(delta_time);
    }
}

void GameObject::SetGeometry(shared_ptr<::Geometry> geometry)
{
    geometry_ = geometry;
}

void GameObject::SetTexture(shared_ptr<Texture> texture)
{
    texture_ = texture;
}

void GameObject::SetMaterial(shared_ptr<IMaterial> material)
{
    material_ = material;
}

void GameObject::AddChild(shared_ptr<GameObject> object)
{
    ASSERT_MSG(object, "Cannot add null child to GameObject");
    object->Transform()->SetParent(Transform());
    children_.push_back(object);
}

bool GameObject::Active() const
{
    return active_;
}

shared_ptr<::Transform> GameObject::Transform()
{
    return transform_;
}

shared_ptr<::Geometry> GameObject::Geometry()
{
    return geometry_;
}

mat4 GameObject::BuildModelMatrix()
{
    return transform_->GetLocalToWorldMatrix();
}
