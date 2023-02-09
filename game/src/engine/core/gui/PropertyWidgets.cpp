#include "engine/core/gui/PropertyWidgets.h"

#include <fmt/format.h>

#include <glm/gtc/type_ptr.hpp>
#include <limits>

using glm::vec3;
using std::string;

static constexpr float kFloatMin = std::numeric_limits<float>::min();
static constexpr float kFloatMax = std::numeric_limits<float>::max();

static const char* VecInputId(const string& label, const string& component);

const char* VecInputId(const string& label, const string& component)
{
    
}

namespace gui
{

void Vec3Field(const string& label, vec3& value)
{
    ImGui::Text(label.c_str());
    fmt::format("{}##vec_x", label);

    ImGui::DragFloat("##vec_x", &value.x, 1.0f, kFloatMin, kFloatMax,
                     "X: %.3f");
    
    ImGui::SameLine();
    ImGui::DragFloat("##vec_y", &value.y);
    
    ImGui::SameLine();
    ImGui::DragFloat("##vec_y", &value.y);
}

};  // namespace gui
