#include "engine/render/passes/depth/ShadowMap.h"

#include <glm/gtc/matrix_transform.hpp>
#include <limits>

using glm::mat4;
using glm::uvec2;
using glm::vec3;
using glm::vec4;

static constexpr float kFloatMax = std::numeric_limits<float>::max();
static constexpr float kFloatMin = std::numeric_limits<float>::lowest();

ShadowMap::ShadowMap(const ShadowMapParams& params)
    : fbo_(),
      depth_map_(),
      params_(params),
      target_pos_(0.0f),
      source_pos_(0.0f),
      light_proj_(1.0f),
      light_view_(1.0f),
      light_projview_(1.0f),
      camera_bounds_(),
      shadow_bounds_()
{
}

void ShadowMap::Init()
{
    // Create depth map texture
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, params_.texture_size.x,
                 params_.texture_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    const float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // Attatch to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           depth_map_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::UpdateBounds(LightParams& light_params,
                             CameraParams& camera_params)
{
    // Build sub-frustum
    const mat4 camera_proj_segment =
        glm::perspective(camera_params.fov_radians, camera_params.aspect_ratio,
                         params_.camera_near_plane, params_.camera_far_plane);

    camera_bounds_.BoundsFromNdcs(camera_proj_segment *
                                  camera_params.view_matrix);

    target_pos_ = camera_bounds_.GetCentroidMidpoint(params_.camera_midpoint_t);
    source_pos_ = target_pos_ + light_params.pos;

    // Build proj and view matrices
    light_view_ = glm::lookAt(source_pos_, target_pos_, light_params.up_dir);

    vec3 min(kFloatMax, kFloatMax, kFloatMax);
    vec3 max(kFloatMin, kFloatMin, kFloatMin);
    const vec3* vertex = camera_bounds_.GetVertexList();

    for (size_t i = 0; i < camera_bounds_.GetVertexCount(); i++)
    {
        const vec4 temp = light_view_ * vec4(*vertex, 1.0f);

        min = glm::min(min, vec3(temp));
        max = glm::max(max, vec3(temp));

        vertex++;
    }

    if (min.x < 0.0f)
    {
        min.x *= params_.bounds_mult.x;
    }
    else
    {
        min.x /= params_.bounds_mult.x;
    }
    if (max.x < 0.0f)
    {
        max.x /= params_.bounds_mult.x;
    }
    else
    {
        max.x *= params_.bounds_mult.x;
    }

    if (min.y < 0.0f)
    {
        min.y *= params_.bounds_mult.y;
    }
    else
    {
        min.y /= params_.bounds_mult.y;
    }
    if (max.y < 0.0f)
    {
        max.y /= params_.bounds_mult.y;
    }
    else
    {
        max.y *= params_.bounds_mult.y;
    }

    if (min.z < 0.0f)
    {
        min.z *= params_.bounds_mult.z;
    }
    else
    {
        min.z /= params_.bounds_mult.z;
    }
    if (max.z < 0.0f)
    {
        max.z /= params_.bounds_mult.z;
    }
    else
    {
        max.z *= params_.bounds_mult.z;
    }

    max = glm::clamp(max, params_.bounds_min, params_.bounds_max);
    min = glm::clamp(min, params_.bounds_min, params_.bounds_max);

    // const float near_plane = glm::min(-25.0f, min.z);
    // const float far_plane = glm::max(125.0f, max.z);

    min.z = params_.bounds_min.z;
    max.z = params_.bounds_max.z;

    light_proj_ = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
    light_projview_ = light_proj_ * light_view_;

    shadow_bounds_.BoundsFromNdcs(light_projview_);
}

void ShadowMap::Prepare()
{
    glViewport(0, 0, params_.texture_size.x, params_.texture_size.y);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (params_.cull_face)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

ShadowMapParams& ShadowMap::GetParams()
{
    return params_;
}

const TextureHandle& ShadowMap::GetTexture() const
{
    return depth_map_;
}

const Cuboid& ShadowMap::GetCameraBounds() const
{
    return camera_bounds_;
}

const Cuboid& ShadowMap::GetShadowBounds() const
{
    return shadow_bounds_;
}

const mat4& ShadowMap::GetTransformation() const
{
    return light_projview_;
}

const vec3& ShadowMap::GetTargetPos() const
{
    return target_pos_;
}