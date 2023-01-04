#include "core/ShaderManager.h"

#include "core/Utils.h"

using std::make_unique;
using std::unique_ptr;
using std::vector;

ShaderManager* ShaderManager::instance_ = nullptr;

void ShaderManager::Init()
{
    ASSERT_MSG(!instance_, "Cannot init shader manager more than once");

    instance_ = new ShaderManager();
    instance_->CreateShaders();
}

void ShaderManager::Destroy()
{
    ASSERT_MSG(instance_, "Cannot destroy if it doesn't exist");
    delete instance_;
}

ShaderManager::ShaderManager() : programs_{}
{
    ASSERT_MSG(!instance_ || (instance_ && instance_ == this),
               "Can only have one ShaderManager instance");
}

ShaderManager& ShaderManager::Instance()
{
    return *instance_;
}

void ShaderManager::CreateShaders()
{
    size_t count = static_cast<size_t>(ShaderType::kCount);
    programs_.reserve(count);

    AddProgram(ShaderType::kBasic,
               make_unique<ShaderProgram>("shaders/default.vert",
                                          "shaders/basic.frag"));
    AddProgram(ShaderType::kTextured,
               make_unique<ShaderProgram>("shaders/default.vert",
                                          "shaders/textured.frag"));
    AddProgram(ShaderType::kPhong,
               make_unique<ShaderProgram>("shaders/default.vert",
                                          "shaders/phong.frag"));
    AddProgram(ShaderType::kEarth,
               make_unique<ShaderProgram>("shaders/default.vert",
                                          "shaders/earth.frag"));

    ASSERT_MSG(programs_.size() == count,
               "Incorrect amount of shader programs registered");
}

ShaderProgram& ShaderManager::GetProgram(ShaderType type)
{
    ASSERT_MSG(type < ShaderType::kCount, "Invalid shader type");

    size_t idx = static_cast<size_t>(type);
    ASSERT_MSG(idx < programs_.size(), "Invalid program index");

    return *programs_[idx]->shader;
}

vector<std::unique_ptr<ShaderManager::ShaderData>>& ShaderManager::GetPrograms()
{
    return programs_;
}

void ShaderManager::AddProgram(ShaderType type,
                               unique_ptr<ShaderProgram> program)
{
    ASSERT_MSG(type < ShaderType::kCount, "Invalid shader type");

    auto data = make_unique<ShaderData>();
    data->type = type;
    data->shader = std::move(program);
    programs_.push_back(std::move(data));

    size_t idx = static_cast<size_t>(type);
    ASSERT_MSG(idx == programs_.size() - 1,
               "Shader program type added to incorrect index");
}