#include "engine/render/PointLight.h"

#include "engine/core/gui/PropertyWidgets.h"
#include "engine/render/RenderService.h"

using glm::vec3;

void PointLight::OnInit(const ServiceProvider& service_provider)
{
    render_service_ = &service_provider.GetService<RenderService>();

    transform_ = &GetEntity().GetComponent<Transform>();

    render_service_->RegisterLight(GetEntity());
}

void PointLight::OnDebugGui()
{
    gui::EditColorProperty("Diffuse", diffuse_);
}

void PointLight::OnDestroy()
{
    render_service_->UnregisterLight(GetEntity());
}

std::string_view PointLight::GetName() const
{
    return "PointLight";
}

void PointLight::SetDiffuse(const vec3& diffuse)
{
    diffuse_ = diffuse;
}

const vec3 PointLight::GetDiffuse() const
{
    return diffuse_;
}