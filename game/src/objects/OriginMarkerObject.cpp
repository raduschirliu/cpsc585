#include "objects/OriginMarkerObject.h"

#include <limits>

#include "core/Utils.h"
#include "objects/AxisMarkerObject.h"

using glm::vec3;
using std::make_shared;
using std::numeric_limits;
using std::shared_ptr;
using utils::Lerp;

static constexpr vec3 kXAxis(1.0f, 0.0f, 0.0f);
static constexpr vec3 kYAxis(0.0f, 1.0f, 0.0f);
static constexpr vec3 kZAxis(0.0f, 0.0f, 1.0f);

// Not using numeric_limits since that is significantly larger than anything
// that would ever exist in a scene here, and we don't need the lines going out
// that far for no reason...
static constexpr float kAxisLength = 2000.0f;

// Using same axis coloring convetion as Unity :)
static constexpr vec3 kXAxisColor = utils::colors::kRed;
static constexpr vec3 kYAxisColor = utils::colors::kGreen;
static constexpr vec3 kZAxisColor = utils::colors::kBlue;

OriginMarkerObject::OriginMarkerObject() : GameObject()
{
    AddChild(
        make_shared<AxisMarkerObject>(kXAxis, false, kAxisLength, kXAxisColor));
    AddChild(
        make_shared<AxisMarkerObject>(kYAxis, false, kAxisLength, kYAxisColor));
    AddChild(
        make_shared<AxisMarkerObject>(kZAxis, false, kAxisLength, kZAxisColor));
}
