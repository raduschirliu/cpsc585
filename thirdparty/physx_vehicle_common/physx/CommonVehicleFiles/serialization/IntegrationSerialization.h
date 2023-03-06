#pragma once

#include "vehicle2/PxVehicleAPI.h"

#include "../physxintegration/PhysXIntegration.h"

#if PX_SWITCH
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#elif PX_OSX
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#elif PX_LINUX && PX_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#if (PX_LINUX && PX_CLANG) || PX_SWITCH
#pragma clang diagnostic pop
#endif

namespace snippetvehicle2
{

using namespace physx;
using namespace physx::vehicle2;

bool readPhysxIntegrationParamsFromJsonFile(const char* directory, const char* filename,
	PhysXIntegrationParams& params);

}//namespace snippetvehicle2
