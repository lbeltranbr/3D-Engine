#pragma once
#include "../Buffers/Mesh.h"

namespace SP
{
    // skybox mesh type definition
    using SkyboxMesh = std::unique_ptr<Mesh<SkyboxVertex>>;

    // helps render a skybox mesh
    SP_INLINE void RenderSkyboxMesh(SkyboxMesh& mesh)
    {
#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
        spdlog::info("SKYBOX: RenderSkyboxMesh()");
#endif
#endif
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        mesh->Draw(GL_TRIANGLES);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_CULL_FACE);
    }

    // helps create a skybox mesh
    SP_INLINE SkyboxMesh CreateSkyboxMesh()
    {
        MeshData<SkyboxVertex> data;

        std::vector<glm::vec3> positions =
        {
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0F},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {-1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f}
        };

        // set veritces
        for (uint32_t i = 0; i < positions.size(); ++i)
        {
            data.Vertices.push_back({ positions[i] });
        }
    #ifdef ENABLE_SPDLOG
        #if ENABLE_COMMENTS == 1
                spdlog::info("****************");
                spdlog::info("SKYBOX");
                spdlog::info("****************");
        #endif
    #endif
        return std::make_unique<Mesh<SkyboxVertex>>(data);
    }
}