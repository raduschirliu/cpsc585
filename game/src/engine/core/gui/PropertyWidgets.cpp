#include "engine/core/gui/PropertyWidgets.h"

#include <glm/gtc/type_ptr.hpp>
#include <limits>

using glm::vec2;
using glm::vec3;
using std::string;

static constexpr float kFloatMin = std::numeric_limits<float>::min();
static constexpr float kFloatMax = std::numeric_limits<float>::max();
static constexpr float kIndentWidth = 5.0f;

namespace gui
{

void ViewProperty(const string& label, const vec3& value)
{
    vec3 value_copy = value;

    ImGui::BeginDisabled();
    EditProperty(label, value_copy);
    ImGui::EndDisabled();
}

bool EditProperty(const string& label, vec3& value)
{
    bool dirty = false;
    const float full_width = ImGui::CalcItemWidth();
    const float item_width = full_width / 3.0f;

    ImGui::BeginGroup();
    ImGui::PushID(label.c_str());

    ImGui::PushItemWidth(item_width);
    dirty |= ImGui::DragFloat("##vec3_x", &value.x, 1.0f, kFloatMin, kFloatMax,
                              "X: %.3f");
    ImGui::SameLine();

    dirty |= ImGui::DragFloat("##vec3_y", &value.y, 1.0f, kFloatMin, kFloatMax,
                              "Y: %.3f");
    ImGui::SameLine();

    dirty |= ImGui::DragFloat("##vec3_z", &value.z, 1.0f, kFloatMin, kFloatMax,
                              "Z: %.3f");
    ImGui::SameLine();

    ImGui::Text(label.c_str());

    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::EndGroup();

    return dirty;
}

bool EditProperty(const string& label, vec2& value)
{
    bool dirty = false;
    const float full_width = ImGui::CalcItemWidth();
    const float item_width = full_width / 2.0f;

    ImGui::BeginGroup();
    ImGui::PushID(label.c_str());

    ImGui::PushItemWidth(item_width);
    dirty |= ImGui::DragFloat("##vec2_x", &value.x, 1.0f, kFloatMin, kFloatMax,
                              "X: %.3f");
    ImGui::SameLine();

    dirty |= ImGui::DragFloat("##vec2_y", &value.y, 1.0f, kFloatMin, kFloatMax,
                              "Y: %.3f");
    ImGui::SameLine();

    ImGui::Text(label.c_str());

    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::EndGroup();

    return dirty;
}

void EditColorProperty(const string& label, vec3& value)
{
    ImGui::ColorEdit3(label.c_str(), glm::value_ptr(value));
}

};  // namespace gui
