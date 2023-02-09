#include "engine/render/PointLight.h"

#include "engine/render/RenderService.h"

using glm::vec3;

void PointLight::OnInit(const ServiceProvider& service_provider)
{
    render_service_ = &service_provider.GetService<RenderService>();

    transform_ = &GetEntity().GetComponent<Transform>();
}

void PointLight::OnDebugGui()
{
}

std::string_view PointLight::GetName() const
{
    return "PointLight";
}

void PointLight::SetColor(const vec3& color)
{
    color_ = color;
}

const vec3 PointLight::GetColor() const
{
    return color_;
}