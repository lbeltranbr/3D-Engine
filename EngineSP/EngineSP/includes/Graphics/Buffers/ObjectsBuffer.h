#pragma once
#include "Common/Core.h"


namespace SP
{
    struct ObjectsBuffer
    {
        SP_INLINE ObjectsBuffer(int32_t width, int32_t height) :
            m_Width(width), m_Height(height), m_Samples(8)
        {
            glGenFramebuffers(1, &m_BufferID);
            glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

            CreateColorAttachment();
			CreateBaseColorAttachment();
			CreateNormalAttachment();
            CreateBrightnessAttachment();
			CreateRenderBuffer();

			CreateDepthAttachment();


            // Attachment Targets
            uint32_t attachments[4] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3
            };

            glDrawBuffers(4, attachments);

            // check frame buffer
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
#ifdef ENABLE_SPDLOG
                spdlog::error("glCheckFramebufferStatus() Failed!");
#endif
            }

            // unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        SP_INLINE ~ObjectsBuffer()
        {
            glDeleteTextures(1, &m_Color);
            glDeleteTextures(1, &m_BaseColor);
            glDeleteTextures(1, &m_Normal);
            glDeleteTextures(1, &m_Brightness);
            glDeleteTextures(1, &m_Depth);
            glDeleteRenderbuffers(1, &m_Render);
            glDeleteFramebuffers(1, &m_BufferID);
        }

        SP_INLINE void Resize(int32_t width, int32_t height)
        {
			// update size     
			m_Width = width;
			m_Height = height;

			// Resize Color Buffer
			glBindTexture(GL_TEXTURE_2D, m_Color);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

			// Resize BaseColor Buffer
			glBindTexture(GL_TEXTURE_2D, m_BaseColor);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            
            // Resize Normal Buffer
			glBindTexture(GL_TEXTURE_2D, m_Normal);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);

			// Resize Brightness Buffer
			glBindTexture(GL_TEXTURE_2D, m_Brightness);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

			// Resize Depth Buffer
			glBindTexture(GL_TEXTURE_2D, m_Depth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			// Resize Render Buffer
			glBindRenderbuffer(GL_RENDERBUFFER, m_Render);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
            
        }

        SP_INLINE uint32_t GetTexture()
        {
            return m_Color;
        }
		SP_INLINE uint32_t GetBaseColorTexture()
		{
			return m_BaseColor;
		}
		SP_INLINE uint32_t GetNormalTexture()
		{
            return m_Normal;
		}

        SP_INLINE uint32_t GetBrightnessMap()
        {
            return m_Brightness;
        }
		SP_INLINE uint32_t GetDepthMap()
		{
			return m_Depth;
		}

		SP_INLINE uint32_t GetBufferID()
		{
			return m_BufferID;
		}

        SP_INLINE int32_t Height()
        {
            return m_Height;
        }
		SP_INLINE int32_t HalfHeight()
		{
			return m_Height/2;
		}

        SP_INLINE int32_t Width()
        {
            return m_Width;
        }
        SP_INLINE int32_t HalfWidth()
        {
            return m_Width/2;
        }

        SP_INLINE float Ratio()
        {
            return (float)m_Width / (float)m_Height;
        }

        SP_INLINE void Begin() //this works
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
            glViewport(0, 0, m_Width, m_Height);
            glClearColor(0, 0, 0, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_MULTISAMPLE);
            glCullFace(GL_BACK);
        }

        SP_INLINE void End()
        {
            glDisable(GL_MULTISAMPLE);
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

    private:

        SP_INLINE void CreateColorAttachment()
        {
            glGenTextures(1, &m_Color);
            glBindTexture(GL_TEXTURE_2D, m_Color);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Color, 0); 

        }
		SP_INLINE void CreateBaseColorAttachment()
		{
			glGenTextures(1, &m_BaseColor);
			glBindTexture(GL_TEXTURE_2D, m_BaseColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BaseColor, 0);

		}
		SP_INLINE void CreateNormalAttachment()
		{
			glGenTextures(1, &m_Normal);
			glBindTexture(GL_TEXTURE_2D, m_Normal);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_Normal, 0);

		}
        SP_INLINE void CreateBrightnessAttachment()
        {
            glGenTextures(1, &m_Brightness);
            glBindTexture(GL_TEXTURE_2D, m_Brightness);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_Brightness, 0);
        }
		SP_INLINE void CreateDepthAttachment()
		{
			glGenTextures(1, &m_Depth);
			glBindTexture(GL_TEXTURE_2D, m_Depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_Depth, 0);
		}

        SP_INLINE void CreateRenderBuffer()
        {
			glGenRenderbuffers(1, &m_Render);
			glBindRenderbuffer(GL_RENDERBUFFER, m_Render);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Render);
        }

    private:
        uint32_t m_BufferID = 0u;
        uint32_t m_Render = 0u;
        uint32_t m_Color = 0u;
		uint32_t m_BaseColor = 0u;
		uint32_t m_Normal = 0u;
		uint32_t m_Brightness = 0u;
		uint32_t m_Depth = 0u;


        int32_t m_Height = 0;
        int32_t m_Width = 0;
        int32_t m_Samples = 0;

    };
}