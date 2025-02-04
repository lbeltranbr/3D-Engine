#pragma once
#include "Shader.h"
#include "../Utilities/Quad.h"

namespace SP
{
    struct FinalShader : Shader
    {
        SP_INLINE FinalShader(const std::string& filename) :
            Shader(filename)
        {
            u_Map = glGetUniformLocation(m_ShaderID, "u_map");
            u_Sky = glGetUniformLocation(m_ShaderID, "u_sky");
            u_Bloom = glGetUniformLocation(m_ShaderID, "u_bloom");
            m_Quad = CreateQuad2D();

            #ifdef ENABLE_SPDLOG
                #if ENABLE_COMMENTS == 1
                    spdlog::info("FINAL: Created Quad2D");
                #endif
            #endif
        }

        SP_INLINE void Render(uint32_t map, uint32_t sky, uint32_t bloom, bool useBloom)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            #ifdef ENABLE_SPDLOG
                #if ENABLE_COMMENTS == 1
                    spdlog::info("////////////////////");
                    spdlog::info("FINAL SHADER");
                    spdlog::info("////////////////////");

                #endif
            #endif

            glUseProgram(m_ShaderID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, map);
            glUniform1i(u_Map, 0);

            if (useBloom) {
				// set bloom map
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, bloom);
				glUniform1i(u_Bloom, 1);
            }

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, sky);
			glUniform1i(u_Sky, 2);

            m_Quad->Draw(GL_TRIANGLES);

            #ifdef ENABLE_SPDLOG
                #if ENABLE_COMMENTS == 1
                    spdlog::info("FINAL: Render Quad2D");
                #endif
            #endif

            glUseProgram(0);
        }

    private:
        
        uint32_t u_Map = 0u;
        uint32_t u_Sky = 0u;
        uint32_t u_Bloom = 0u;
        Quad2D m_Quad;
    };
}