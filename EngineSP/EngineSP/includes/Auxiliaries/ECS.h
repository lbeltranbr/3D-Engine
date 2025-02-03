#pragma once
#include <../entt/entt.hpp>
#include "Graphics/Utilities/Data.h"
#include "Graphics/Models/Model.h"

namespace SP
{
    // skybox component
    struct SkyboxComponent
    {
        SP_INLINE SkyboxComponent(const SkyboxComponent&) = default;
        SP_INLINE SkyboxComponent() = default;
        Skybox Sky;
    };
    // transform component
    struct TransformComponent
    {
        SP_INLINE TransformComponent(const TransformComponent&) = default;
        SP_INLINE TransformComponent() = default;
        Transform3D Transform;
    };

    // camera component
    struct CameraComponent
    {
        SP_INLINE CameraComponent(const CameraComponent&) = default;
        SP_INLINE CameraComponent() = default;
        Camera3D Camera;
    };

    // common component
    struct EnttComponent
    {
        SP_INLINE EnttComponent(const EnttComponent&) = default;
        SP_INLINE EnttComponent() = default;
        std::string Name = "Untitled";
    };
    // model component
    struct ModelComponent
    {
        SP_INLINE ModelComponent(const ModelComponent&) = default;
        SP_INLINE ModelComponent() = default;
        Model3D Model;
        PbrMaterial Material;
    };
    // mesh component
    struct MeshComponent
    {
        SP_INLINE MeshComponent(const MeshComponent&) = default;
        SP_INLINE MeshComponent() = default;
        Mesh3D Mesh;
    };
    // point light component
    struct PointLightComponent
    {
        SP_INLINE PointLightComponent(const PointLightComponent&) = default;
        SP_INLINE PointLightComponent() = default;
        PointLight Light;
    };
    // point light component
    struct SpotLightComponent
    {
        SP_INLINE SpotLightComponent(const SpotLightComponent&) = default;
        SP_INLINE SpotLightComponent() = default;
        SpotLight Light;
    };
    // direct light component
    struct DirectLightComponent
    {
        SP_INLINE DirectLightComponent(const DirectLightComponent&) = default;
        SP_INLINE DirectLightComponent() = default;
        DirectLight Light;
    };

    // base entity
    struct Entity
    {
        SP_INLINE Entity(EntityRegistry* registry, EntityID entity) :
            m_Registry(registry), m_EnttID(entity)
        {}

        SP_INLINE Entity(EntityRegistry* registry) :
            m_Registry(registry)
        {
            m_EnttID = m_Registry->create();
        }

        SP_INLINE virtual ~Entity() = default;
        SP_INLINE Entity() = default;

        SP_INLINE operator EntityID ()
        {
            return m_EnttID;
        }

        SP_INLINE operator bool()
        {
            return m_Registry != nullptr &&
                m_Registry->valid(m_EnttID);
        }

        SP_INLINE EntityID ID()
        {
            return m_EnttID;
        }

        // ++

        template<typename T, typename... Args>
        SP_INLINE T& Attach(Args&&... args)
        {
            return m_Registry->get_or_emplace<T>
                (m_EnttID, std::forward<Args>(args)...);
        }

        template<typename T>
        SP_INLINE void Detach()
        {
            m_Registry->remove<T>(m_EnttID);
        }

        SP_INLINE void Destroy()
        {
            if (m_Registry)
            {
                m_Registry->destroy(m_EnttID);
            }
        }

        template<typename T>
        SP_INLINE bool Has()
        {
            return m_Registry != nullptr &&
                m_Registry->all_of<T>(m_EnttID);
        }

        template<typename T>
        SP_INLINE T& Get()
        {
            return m_Registry->get<T>(m_EnttID);
        }

    protected:
        EntityRegistry* m_Registry = nullptr;
        EntityID m_EnttID = NENTT;
    };
}