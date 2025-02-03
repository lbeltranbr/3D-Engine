#pragma once
#include "Common/Core.h"
#include "../Textures/Texture.h"

namespace SP
{
    // skybox data
    struct Skybox
    {
        SP_INLINE Skybox(const Skybox&) = default;
        SP_INLINE Skybox() = default;
 
        uint32_t CubeMap = 0u;
        uint32_t IrradMap = 0u;
        uint32_t PrefilMap = 0u;
        uint32_t BrdfMap = 0u;
    };
    // pbr material
    struct PbrMaterial
    {
        SP_INLINE PbrMaterial(const PbrMaterial&) = default;
        SP_INLINE PbrMaterial() = default;

        glm::vec3 Albedo = glm::vec3(1.0);
        glm::vec3 Emissive = glm::vec3(0.0);

        float Roughness = 0.5f;
        float Metalness = 0.0f;
        float Occlusion = 0.0f;

        Texture OcclusionMap;
        Texture RoughnessMap;
        Texture MetalnessMap;
        Texture EmissiveMap;
        Texture AlbedoMap;
        Texture NormalMap;
    };
    // point light
    struct PointLight
    {
        SP_INLINE PointLight(const PointLight&) = default;
        SP_INLINE PointLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
    }; 
    // directional light
    struct DirectLight
    {
        SP_INLINE DirectLight(const DirectLight&) = default;
        SP_INLINE DirectLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float ShadowBias = 0.05f;
        float Intensity = 1.0f;
    };
    // spot light
    struct SpotLight
    {
        SP_INLINE SpotLight(const SpotLight&) = default;
        SP_INLINE SpotLight() = default;

        glm::vec3 Radiance = glm::vec3(1.0f);
        float Intensity = 1.0f;
        float FallOff = 60.5f;
        float CutOff = 20.0f;
    };
    // transform
    struct Transform3D
    {
        SP_INLINE Transform3D(const Transform3D&) = default;
        SP_INLINE Transform3D() = default;

        SP_INLINE glm::mat4 Matrix() const
        {
            return (glm::translate(glm::mat4(1.0f), Translate) *
                glm::toMat4(glm::quat(glm::radians(Rotation))) *
                glm::scale(glm::mat4(1.0f), Scale));
        }

        glm::vec3 Translate = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);
    };

    // camera
    struct Camera3D
    {
        SP_INLINE Camera3D(const Camera3D&) = default;
        SP_INLINE Camera3D() = default;

        SP_INLINE glm::mat4 Frustum(const Transform3D& transform, float ratio) const
        {
            return Projection(ratio) * View(transform);
        }

        SP_INLINE glm::mat4 View(const Transform3D& transform) const
        {
            return glm::lookAt(transform.Translate, (transform.Translate +
                glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::toMat4(glm::quat(glm::radians(transform.Rotation)));
        }

        SP_INLINE glm::mat4 Projection(float ratio) const
        {
            return glm::perspective(FOV, ratio, NearPlane, FarPlane);
        }

        float NearPlane = 0.3000f;
        float FarPlane = 1000.0f;
        float FOV = 45.0f;
    };

}