#include "Checkpoints.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <ostream>
#include <sstream>
#include <unordered_set>
#include <utility>

#include "engine/core/debug/Assert.h"
#include "engine/core/debug/Log.h"
#include "engine/core/json/deserialize_utils.h"

using glm::vec3;
using rapidjson::Document;
using rapidjson::IStreamWrapper;
using rapidjson::StringBuffer;
using rapidjson::Value;
using rapidjson::Writer;
using std::pair;
using std::string;
using std::vector;

static vector<CheckpointEntry> kCheckpoints;
static const string kCheckpointFilePath = "resources/scene/checkpoints.jsonc";

bool CheckpointEntry::Deserialize(const Value& node)
{
    bool status = true;
    status &= json::GetVec3(node, "position", position);
    status &= json::GetVec3(node, "orientation", orientation);

    // Size is optional
    size = vec3(70.0f, 10.0f, 10.0f);
    json::GetVec3(node, "size", size);

    return status;
}

void Checkpoints::LoadCheckpointFile()
{
    /*
    // Used for converting obj checkpoint format to JSON
    
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);

    writer.StartArray();

    for (auto& checkpoint : kSortedCheckpoints)
    {
        writer.StartObject();

        writer.Key("position");
        writer.StartArray();
        writer.Double(checkpoint.first.x);
        writer.Double(checkpoint.first.y);
        writer.Double(checkpoint.first.z);
        writer.EndArray();

        writer.Key("orientation");
        writer.StartArray();
        writer.Double(checkpoint.second.x);
        writer.Double(checkpoint.second.y);
        writer.Double(checkpoint.second.z);
        writer.EndArray();

        writer.EndObject();
    }

    writer.EndArray();

    std::ofstream ofile("resources/checkpoints.jsonc");
    ofile << strbuf.GetString();
    ofile.close();
    */

    std::ifstream file_stream(kCheckpointFilePath);
    ASSERT_MSG(file_stream.is_open(), "Failed to open checkpoint file");

    IStreamWrapper stream(file_stream);
    Document doc;
    doc.ParseStream(stream);

    if (doc.HasParseError())
    {
        debug::LogError("JSON parsing error: {}", doc.GetParseError());
        ASSERT(false);
    }

    ASSERT_MSG(doc.IsArray(), "Asset file must be an object");

    auto doc_array = doc.GetArray();
    for (Value* value = doc_array.Begin(); value != doc_array.End(); value++)
    {
        ASSERT(value);

        CheckpointEntry entry;
        bool status = entry.Deserialize(*value);
        ASSERT(status);

        kCheckpoints.push_back(entry);
    }

    debug::LogInfo("Loaded {} checkpoints from file", kCheckpoints.size());
}

const vector<CheckpointEntry>& Checkpoints::GetCheckpoints()
{
    return kCheckpoints;
}