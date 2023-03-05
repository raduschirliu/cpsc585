#include "IntegrationSerialization.h"
#include "SerializationCommon.h"

namespace snippetvehicle2
{

bool readPhysxIntegrationParamsFromJsonFile(const char* directory, const char* filename, PhysXIntegrationParams& params)
{
	rapidjson::Document config;
	if (!openDocument(directory, filename, config))
    {
		return false;
    }

    if (!config.HasMember("ActorCMassLocalPose") ||
        !config.HasMember("ActorBoxShapeHalfExtents") ||
        !config.HasMember("ActorBoxShapeLocalPose"))
    {
        return false;
    }

    if (!readTransform(config["ActorCMassLocalPose"], params.physxActorCMassLocalPose))
    {
        return false;
    }

    if (!readVec3(config["ActorBoxShapeHalfExtents"], params.physxActorBoxShapeHalfExtents))
    {
        return false;
    }

    if (!readTransform(config["ActorBoxShapeLocalPose"], params.physxActorBoxShapeLocalPose))
    {
        return false;
    }

	return true;
}

}