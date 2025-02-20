#pragma once
#include "Shader.h"
#include "../Utilities/Quad.h"

namespace SP
{
	struct GBufferShader : Shader
	{
		SP_INLINE GBufferShader(const std::string& filename) :
			Shader(filename)
		{
			u_Map = glGetUniformLocation(m_ShaderID, "u_map");
			u_depth = glGetUniformLocation(m_ShaderID, "u_depth");
			m_Quad = CreateQuad2D();

#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
			spdlog::info("GBuffer: Created Quad2D");
#endif
#endif
		}

		SP_INLINE void Render(uint32_t map, int depth)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClearColor(0, 0, 0, 1);
			//glClear(GL_COLOR_BUFFER_BIT);

#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
			spdlog::info("////////////////////");
			spdlog::info("GBuffer SHADER");
			spdlog::info("////////////////////");

#endif
#endif

			glUseProgram(m_ShaderID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, map);
			glUniform1i(u_Map, 0);

			glUniform1i(u_depth, depth);

			m_Quad->Draw(GL_TRIANGLES);

#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
			spdlog::info("GBuffer: Render Quad2D");
#endif
#endif

			glUseProgram(0);
		}

	private:

		uint32_t u_Map = 0u;
		uint32_t u_depth = 0u;
		Quad2D m_Quad;
	};
}