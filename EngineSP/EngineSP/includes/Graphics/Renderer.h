#pragma once
#include "Buffers/ObjectsBuffer.h"
#include "Buffers/FrameBuffer.h"
#include "Shaders/Final.h"
#include "Shaders/PBR.h"
#include "Shaders/SkyBox.h"
#include "Shaders/SkyMap.h"
#include "Shaders/Irradiance.h"
#include "Shaders/Prefiltered.h"
#include "Shaders/BRDF.h"
#include "Shaders/Shadow.h"
#include "Shaders/Bloom.h"
#include "Shaders/GBuffer.h"

#include <iostream>


namespace SP {

	struct GraphicsRenderer {

		SP_INLINE GraphicsRenderer(int32_t width, int32_t height) {

			glewExperimental = GL_TRUE;

			if (glewInit() != GLEW_OK) {

			#ifdef ENABLE_SPDLOG
				spdlog::critical("failed to init glew!");
			#endif

				exit(EXIT_FAILURE);
			}

			
			m_SkyFrame = std::make_unique<FrameBuffer>(width, height);
			m_ObjectFrame = std::make_unique<ObjectsBuffer>(width, height);

			m_Final = std::make_unique<FinalShader>("Resources/Shaders/final.glsl");
			m_GBuffer = std::make_unique<GBufferShader>("Resources/Shaders/gbuffer.glsl");

			m_Skybox = std::make_unique<SkyboxShader>("Resources/Shaders/skybox.glsl");
			m_SkyMap = std::make_unique<SkyMapShader>("Resources/Shaders/skymap.glsl");
			m_Irrad = std::make_unique<IrradianceShader>("Resources/Shaders/irradiance.glsl");
			m_Prefil = std::make_unique<PrefilteredShader>("Resources/Shaders/prefiltered.glsl");
			m_Brdf = std::make_unique<BrdfShader>("Resources/Shaders/brdf.glsl");

			m_Pbr = std::make_unique<PbrShader>("Resources/Shaders/pbr.glsl");
			m_Shadow = std::make_unique<ShadowShader>("Resources/Shaders/shadow.glsl");

			m_Bloom = std::make_unique<BloomShader>("Resources/Shaders/bloom.glsl", width, height);

			m_SkyboxMesh = CreateSkyboxMesh();
		}
		//LIGHTS
		SP_INLINE void SetPointLight(PointLight& light, Transform3D& transform, uint32_t index)
		{
			m_Pbr->SetPointLight(light, transform, index);
		}
		SP_INLINE void SetPointLightCount(int32_t count)
		{
			m_Pbr->SetPointLightCount(count);
		}
		SP_INLINE void SetDirectLight(DirectLight& light, Transform3D& transform, uint32_t index)
		{
			m_Pbr->SetDirectLight(light, transform, index);
		}
		SP_INLINE void SetDirectLightCount(int32_t count)
		{
			m_Pbr->SetDirectLightCount(count);
		}
		SP_INLINE void SetSpotLight(SpotLight& light, Transform3D& transform, uint32_t index)
		{
			m_Pbr->SetSpotLight(light, transform, index);
		}
		SP_INLINE void SetSpotLightCount(int32_t count)
		{
			m_Pbr->SetSpotLightCount(count);
		}
		//CAMERA
		SP_INLINE void SetCamera(Camera3D& camera, Transform3D& transform)
		{
			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
						spdlog::info("RENDERER: SetCamera()");
				#endif
			#endif
						float aspect = m_ObjectFrame->Ratio();

			m_Skybox->Bind();
			m_Skybox->SetCamera(camera, transform, aspect);
			m_Pbr->Bind(); //start using PBR shader
			m_Pbr->SetCamera(camera, transform, aspect);

		}
		//SKYBOX
		SP_INLINE void InitSkybox(Skybox& sky, Texture& texture, int32_t size)
		{

			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
					spdlog::info("RENDERER: Generate SkyMap");
				#endif
			#endif
			
			sky.CubeMap = m_SkyMap->Generate(texture, m_SkyboxMesh, size);

			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
					spdlog::info("RENDERER: Generate IrradMap");
				#endif
			#endif

			sky.IrradMap = m_Irrad->Generate(sky.CubeMap, m_SkyboxMesh, 32);

			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
					spdlog::info("RENDERER: Generate PrefilteredMap");
				#endif
			#endif
			
			sky.PrefilMap = m_Prefil->Generate(sky.CubeMap, m_SkyboxMesh, 128);

		#ifdef ENABLE_SPDLOG
			#if ENABLE_COMMENTS == 1
				spdlog::info("RENDERER: Generate BrdfMap");
			#endif
		#endif

			sky.BrdfMap = m_Brdf->Generate(512);

		}
		SP_INLINE void DrawSkybox(Skybox& sky, Transform3D& transform)
		{
			
			m_Skybox->Bind();
			m_Skybox->Draw(m_SkyboxMesh, sky.CubeMap, transform);
			m_SkyFrame->End();

			m_ObjectFrame->Begin();
			m_Pbr->Bind();
			m_Pbr->SetEnvMaps(sky.IrradMap, sky.PrefilMap, sky.BrdfMap, m_Shadow->GetDepthMap());
			
		}
		//DRAW
		SP_INLINE void Draw(Model3D& model, PbrMaterial& material, Transform3D& transform)
		{
			m_Pbr->Bind();//not needed for now, be careful when you add new shaders
			// Render object
			m_Pbr->Draw(model, material, transform);

		}
		SP_INLINE void DrawDepth(Model3D& model, Transform3D& transform)
		{
			m_Shadow->Draw(model, transform);
		}
		//RESIZE
		SP_INLINE void Resize(int32_t width, int32_t height)
		{

		#ifdef ENABLE_SPDLOG
			#if ENABLE_COMMENTS == 1
				spdlog::info("RENDERER: Resize()");
			#endif
		#endif

				m_ObjectFrame->Resize(width, height);
				m_SkyFrame->Resize(width, height);
		}
		//FRAME
		SP_INLINE uint32_t GetFrame()
		{

		#ifdef ENABLE_SPDLOG
			#if ENABLE_COMMENTS == 1
				spdlog::info("RENDERER: GetFrame()");
			#endif
		#endif

			return m_ObjectFrame->GetTexture();
		}
		SP_INLINE void NewFrame()
		{
			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
					spdlog::info(" ");
					spdlog::info("-------------------------------------------");
					spdlog::info("RENDERER: New Frame");
				#endif
			#endif
			
			//Bind FrameBuffer and clear it
			m_SkyFrame->Begin(); 
		}
		SP_INLINE void EndFrame()
		{
			
			m_ObjectFrame->End(); //we close frame buffer here
			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
					spdlog::info("RENDERER: End Frame");
				#endif
			#endif

			m_Bloom->Bind();
			m_Bloom->Compute(m_ObjectFrame->GetBrightnessMap(), 10);

		}
		SP_INLINE void ShowFrame(bool gBuffer)
		{
#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
			spdlog::info("RENDERER: ShowFrame()");
#endif
#endif
			if (gBuffer) {
				//top-left
				glViewport(0, m_ObjectFrame->HalfHeight(), m_ObjectFrame->HalfWidth(), m_ObjectFrame->HalfHeight());
				m_Final->Render(m_ObjectFrame->GetTexture(), m_SkyFrame->GetTexture(), m_Bloom->GetMap(), true);

				//top-right
				glViewport(m_ObjectFrame->HalfWidth(), m_ObjectFrame->HalfHeight(), m_ObjectFrame->HalfWidth(), m_ObjectFrame->HalfHeight());
				m_GBuffer->Render(m_ObjectFrame->GetBaseColorTexture(),0);

				//bottom-left
				glViewport(0, 0, m_ObjectFrame->HalfWidth(), m_ObjectFrame->HalfHeight());
				m_GBuffer->Render(m_ObjectFrame->GetNormalTexture(),0);

				//bottom-right
				glViewport(m_ObjectFrame->HalfWidth(), 0, m_ObjectFrame->HalfWidth(), m_ObjectFrame->HalfHeight());
				m_GBuffer->Render(m_ObjectFrame->GetDepthMap(),1);
			}
			else {
				glViewport(0, 0, m_ObjectFrame->Width(), m_ObjectFrame->Height());
				m_Final->Render(m_ObjectFrame->GetTexture(), m_SkyFrame->GetTexture(), m_Bloom->GetMap(), true);
			}
			
		}
		SP_INLINE void BeginShadowPass(const glm::vec3& LightDir)
		{
			// prepare projection and view mtx
			static auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
			auto view = glm::lookAt(LightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			// compute light space
			auto lightSpaceMtx = (proj * view);

			// set pbr shader light space mtx and depth map
			m_Pbr->Bind();
			m_Pbr->SetLightSpaceMatrix(lightSpaceMtx);

			// begin depth rendering
			m_Shadow->Bind();
			m_Shadow->BeginFrame(lightSpaceMtx);
		}
		SP_INLINE void EndShadowPass()
		{
			m_Shadow->EndFrame();
		}

	private:

		std::unique_ptr<FinalShader> m_Final;
		std::unique_ptr<ObjectsBuffer> m_ObjectFrame;
		std::unique_ptr<FrameBuffer> m_SkyFrame;

		std::unique_ptr<PbrShader> m_Pbr;
		std::unique_ptr<SkyboxShader> m_Skybox;
		std::unique_ptr<SkyMapShader> m_SkyMap;
		std::unique_ptr<IrradianceShader> m_Irrad;
		std::unique_ptr<PrefilteredShader> m_Prefil;
		std::unique_ptr<BrdfShader> m_Brdf;
		std::unique_ptr<ShadowShader> m_Shadow;
		std::unique_ptr<BloomShader> m_Bloom;
		std::unique_ptr<GBufferShader> m_GBuffer;

		SkyboxMesh m_SkyboxMesh;
	};
}