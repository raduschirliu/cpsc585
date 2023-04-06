#pragma once

#include <glm/glm.hpp>

#include "engine/core/gfx/GLHandles.h"
#include "engine/core/math/Cuboid.h"

struct ShadowMapParams
{
    glm::uvec2 texture_size;
    float camera_near_plane;
    float camera_far_plane;
    float camera_midpoint_t;
    glm::vec3 bounds_min;
    glm::vec3 bounds_max;
    glm::vec3 bounds_mult;
    bool cull_face;
};

class ShadowMap
{
  public:
    struct LightParams
    {
        glm::vec3 up_dir;
        glm::vec3 pos;
    };

    struct CameraParams
    {
        glm::vec3 pos;
        glm::mat4 view_matrix;
        float fov_radians;
        float aspect_ratio;
    };

    ShadowMap(const ShadowMapParams& params);
    ~ShadowMap() = default;

    // Disallow copying
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap operator=(const ShadowMap&) = delete;

    // Allow moving
    ShadowMap(ShadowMap&& other) noexcept;
    ShadowMap& operator=(ShadowMap&& other) noexcept;

    void Init();
    void UpdateBounds(LightParams& light_params, CameraParams& camera_params);
    void Prepare();

    ShadowMapParams& GetParams();

    const TextureHandle& GetTexture() const;
    const Cuboid& GetCameraBounds() const;
    const Cuboid& GetShadowBounds() const;
    const glm::mat4& GetTransformation() const;
    const glm::vec3& GetTargetPos() const;

  private:
    FramebufferHandle fbo_;
    TextureHandle depth_map_;
    ShadowMapParams params_;
    glm::vec3 target_pos_;
    glm::vec3 source_pos_;
    glm::mat4 light_proj_;
    glm::mat4 light_view_;
    glm::mat4 light_projview_;
    Cuboid camera_bounds_;
    Cuboid shadow_bounds_;
};
