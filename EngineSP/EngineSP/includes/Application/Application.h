#pragma once
#include "Interface.h"

namespace SP {

	struct Application : AppInterface {

		SP_INLINE Application() {

			m_LayerID = TypeID<Application>();
			m_Context = new AppContext();

			AttachCallback<WindowResizeEvent>([this](auto e)
				{
					// resize renderer frame buffer
					m_Context->Renderer->Resize(e.Width, e.Height);
				});
		}

		SP_INLINE ~Application() {
			SP_DELETE(m_Context);
		}

		SP_INLINE void RunContext() {


			auto albedo = std::make_shared<Texture2D>("Resources/Textures/Albedo.png", false, false);
			auto albedo2 = std::make_shared<Texture2D>("Resources/Textures/AlbedoBrick.png", false, false);
			auto normal = std::make_shared<Texture2D>("Resources/Textures/Normal.png", false, false);
			auto normal2 = std::make_shared<Texture2D>("Resources/Textures/NormalBrick.png", false, false);
			auto roughness = std::make_shared<Texture2D>("Resources/Textures/Roughness.png", false, false);
			auto roughness2 = std::make_shared<Texture2D>("Resources/Textures/RoughnessBrick.png", false, false);
			auto ambientOcclusion = std::make_shared<Texture2D>("Resources/Textures/AoBrick.png", false, false);

			auto cubeModel = std::make_shared<Model>("Resources/Models/cube.fbx");
			auto sphereModel = std::make_shared<Model>("Resources/Models/sphere.fbx");
			//auto swordModel = std::make_shared<Model>("Resources/Models/sword.fbx");
			//auto plueModel = std::make_shared<Model>("Resources/Models/plue.fbx");

			auto skymap = std::make_shared<Texture2D>("Resources/Textures/HDRs/Sky.hdr", true, true);

			// create scene camera
			auto camera = CreateCamera(glm::vec3(0.0f, 0.0f, 5.0f));

			// skybox entity
			auto skybox = CreateSkybox();

			//lights
			auto dlight1 = CreateLight(DIRECT, glm::vec3(0.0f), glm::vec3(0.0f, 5.0f, -5.0f), 0.1, glm::vec3(1.0f));

			auto plight1 = CreateLight(POINT, glm::vec3(5.0f, 0.0f, -3.0f), glm::vec3(0.0f), 20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			auto splight1 = CreateLight(SPOT, glm::vec3(-4.0f, 1.0f, -3.0f), glm::vec3(0.0f, -90.0f, 0.0f), 20.0f, glm::vec3(0.0f, 1.0f, 0.0f));

			//just a helper to identify the lights
			auto sphereplight = CreateSphere(sphereModel, glm::vec3(5.0f, 0.0f, -3.0f), glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), false);
			auto spheresplight = CreateSphere(sphereModel, glm::vec3(-4.0f, 1.0f, -3.0f), glm::vec3(0.1f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), false);

			//auto plue = CreateEntt<Entity>();
			//auto& modp = plue.Attach<ModelComponent>();
			//modp.Model = plueModel;
			//modp.Material.Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
			//modp.Material.Roughness = 0.9f;
			////modp.Material.Metalness = 0.1f;
			////plue.Attach<TransformComponent>().Transform.Rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
			//plue.Attach<TransformComponent>().Transform.Translate = glm::vec3(2.0f, -0.5f, -3.0f);
			////plue.Get<TransformComponent>().Transform.Scale = glm::vec3(0.5);

			//entities
			auto sphere = CreateSphere(sphereModel, glm::vec3(0.0f, 1.0f, -5.0f), glm::vec3(1.0f), glm::vec3(0.8f, 0.1f, 0.8f), glm::vec3(1.0f), true);
			auto cube = CreateBox(cubeModel, glm::vec3(0.0f, -2.0f, -5.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(20.0f, 20.0f, 0.5f), albedo2, normal2, roughness2, ambientOcclusion, true);
			auto cube2 = CreateBox(cubeModel, glm::vec3(4.0f, 1.0f, -3.0f), glm::vec3(0.0f), glm::vec3(0.5f), albedo2, nullptr, roughness2, ambientOcclusion, true);

			// generate environment maps
			EnttView<Entity, SkyboxComponent>([this, &skymap](auto entity, auto& comp)
				{
					#ifdef ENABLE_SPDLOG
						#if ENABLE_COMMENTS == 1
								spdlog::info("****************");
								spdlog::info("ENVIRONMENT MAPS");
								spdlog::info("****************");
						#endif
					#endif
					m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
				});

			while (m_Context->Window->PollEvents()) {
				
				RenderSceneDepth();

				m_Context->Renderer->NewFrame();

				EnttView<Entity, CameraComponent>([this](auto entity, auto& comp) {
					auto& transform = entity.template Get<TransformComponent>().Transform;
					m_Context->Renderer->SetCamera(comp.Camera, transform);
					});

				// set shader point lights
				int32_t lightCounter = 0u;
				EnttView<Entity, PointLightComponent>([this, &lightCounter](auto entity, auto& comp)
					{
						auto& transform = entity.template Get<TransformComponent>().Transform;
						m_Context->Renderer->SetPointLight(comp.Light, transform, lightCounter);
						lightCounter++;

					});
				// set number of point lights
				m_Context->Renderer->SetPointLightCount(lightCounter);

				// set shader direct lights
				lightCounter = 0u;
				EnttView<Entity, DirectLightComponent>([this, &lightCounter](auto entity, auto& comp)
					{
						auto& transform = entity.template Get<TransformComponent>().Transform;
						m_Context->Renderer->SetDirectLight(comp.Light, transform, lightCounter);
						lightCounter++;

					});
				// set number of direct lights
				m_Context->Renderer->SetDirectLightCount(lightCounter);

				// set shader spot lights
				lightCounter = 0u;
				EnttView<Entity, SpotLightComponent>([this, &lightCounter](auto entity, auto& comp)
					{
						auto& transform = entity.template Get<TransformComponent>().Transform;
						m_Context->Renderer->SetSpotLight(comp.Light, transform, lightCounter);
						lightCounter++;

					});
				// set number of direct lights
				m_Context->Renderer->SetSpotLightCount(lightCounter);
				
				// render skybox
				EnttView<Entity, SkyboxComponent>([this](auto entity, auto& comp)
					{
						auto& transform = entity.template Get<TransformComponent>().Transform;
						m_Context->Renderer->DrawSkybox(comp.Sky, transform);
					});
				// render models
				EnttView<Entity, ModelComponent>([this](auto entity, auto& comp)
					{
						auto& transform = entity.template Get<TransformComponent>().Transform;
						m_Context->Renderer->Draw(comp.Model, comp.Material, transform);
						//transform.Rotation.y += 0.1f;
					});

				m_Context->Renderer->EndFrame();

				//Update layers
				for (auto layer : m_Context->Layers)
					layer->OnUpdate();

				m_Context->Renderer->ShowFrame();

			}
		}
		 private:

			 SP_INLINE Entity CreateCamera(glm::vec3 position) {
				 
				 Entity camera = CreateEntt<Entity>();
				 camera.Attach<TransformComponent>().Transform.Translate = position;
				 camera.Attach<CameraComponent>();
				
				 return camera;
			 }
			 SP_INLINE Entity CreateDefaultBox(Model3D cubeModel) {

				 Entity cube = CreateEntt<Entity>();
				 auto& mod1 = cube.Attach<ModelComponent>();
				 mod1.Model = cubeModel;
				 cube.Attach<TransformComponent>();
				 return cube;

			 }
			 SP_INLINE Entity CreateBox(Model3D cubeModel, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 albedo, float roughness, float metalness, bool shadowCast) {

				 Entity cube = CreateEntt<Entity>();
				 auto& mod1 = cube.Attach<ModelComponent>();
				 mod1.Model = cubeModel;
				 mod1.Material.Albedo = albedo;
				 mod1.Material.Roughness = roughness;
				 cube.Attach<TransformComponent>().Transform.Rotation = rotation;
				 cube.Get<TransformComponent>().Transform.Scale = scale;
				 cube.Get<TransformComponent>().Transform.Translate = position;
				 return cube;


			 }
			 SP_INLINE Entity CreateBox(Model3D cubeModel, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Texture albedo, Texture normal, Texture roughness, Texture ambientOcclusion, bool shadowCast) {

				 Entity cube = CreateEntt<Entity>();
				 auto& mod1 = cube.Attach<ModelComponent>();
				 mod1.Model = cubeModel;
				 mod1.Material.AlbedoMap = albedo;
				 mod1.Material.NormalMap = normal;
				 mod1.Material.RoughnessMap = roughness;
				 mod1.Material.OcclusionMap = ambientOcclusion;
				 cube.Attach<TransformComponent>().Transform.Rotation = rotation;
				 cube.Get<TransformComponent>().Transform.Scale = scale;
				 cube.Get<TransformComponent>().Transform.Translate = position;
				 return cube;


			 }
			 SP_INLINE Entity CreateDefaultSphere(Model3D sphereModel) {

				 Entity sphere = CreateEntt<Entity>();
				 sphere.Attach<ModelComponent>().Model = sphereModel;
				 return sphere;

			 }
			 SP_INLINE Entity CreateSphere(Model3D sphereModel, glm::vec3 position, glm::vec3 scale, glm::vec3 albedo, glm::vec3 emissive, bool shadowCast) {

				 Entity sphere = CreateEntt<Entity>();
				 auto& mod = sphere.Attach<ModelComponent>();
				 mod.Model = sphereModel;
				 mod.Material.Albedo = albedo;
				 mod.Material.Emissive = emissive;
				 mod.CastShadow = shadowCast;
				 sphere.Attach<TransformComponent>().Transform.Scale = scale;
				 sphere.Get<TransformComponent>().Transform.Translate = position;
				 return sphere;
			 }
			 SP_INLINE Entity CreateLight(LightType type, glm::vec3 position, glm::vec3 rotation, float intensity, glm::vec3 radiance) {
				 
				 Entity light = CreateEntt<Entity>();

				 if (type == DIRECT) {

					 light.Attach<TransformComponent>().Transform.Rotation = rotation;
					 light.Attach<DirectLightComponent>().Light.Intensity = intensity;
					 light.Get<DirectLightComponent>().Light.Radiance = radiance;
					 return light;

				 }
				 if (type == POINT) {

					 light.Attach<TransformComponent>().Transform.Rotation = rotation;
					 light.Get<TransformComponent>().Transform.Translate = position;
					 light.Attach<PointLightComponent>().Light.Intensity = intensity;
					 light.Get<PointLightComponent>().Light.Radiance = radiance;

					 return light;

				 }
				 if (type == SPOT) {

					 light.Attach<TransformComponent>().Transform.Rotation = rotation;
					 light.Get<TransformComponent>().Transform.Translate = position;
					 light.Attach<SpotLightComponent>().Light.Intensity = intensity;
					 light.Get<SpotLightComponent>().Light.Radiance = radiance;

					 return light;

				 }
			 }
			 SP_INLINE Entity CreateSkybox() {

				 Entity skybox = CreateEntt<Entity>();
				 skybox.Attach<TransformComponent>();
				 skybox.Attach<SkyboxComponent>();
				 return skybox;
			 }
			 SP_INLINE void RenderSceneDepth()
			 {
				 EnttView<Entity, DirectLightComponent>([this](auto light, auto&)
					 {
						 // light direction
						 auto& lightDir = light.template Get<TransformComponent>().Transform.Rotation;
						 /*float delta = 0.0166666f;

						 if (m_Context->Window->IsKey(GLFW_KEY_A))
							 lightDir.x -= delta;
						 if (m_Context->Window->IsKey(GLFW_KEY_D))
							 lightDir.x += delta;
						 if (m_Context->Window->IsKey(GLFW_KEY_W))
							 lightDir.y -= delta;
						 if (m_Context->Window->IsKey(GLFW_KEY_S))
							 lightDir.y += delta;
						 if (m_Context->Window->IsKey(GLFW_KEY_E))
							 lightDir.z -= delta;
						 if (m_Context->Window->IsKey(GLFW_KEY_R))
							 lightDir.z += delta;*/

						 // begin rendering
						 m_Context->Renderer->BeginShadowPass(lightDir);

						 // render depth 
						 EnttView<Entity, ModelComponent>([this, &lightDir](auto entity, auto& comp)
							 {
								 
								 if (comp.CastShadow) {
									 auto& transform = entity.template Get<TransformComponent>().Transform;
									 m_Context->Renderer->DrawDepth(comp.Model, transform);
								 }

							/*	 float delta = 0.0166666f;
								 if (4 == (uint32_t)entity.ID())
								 {
									 if (m_Context->Window->IsKey(GLFW_KEY_LEFT))
										 transform.Translate.x -= delta;
									 if (m_Context->Window->IsKey(GLFW_KEY_RIGHT))
										 transform.Translate.x += delta;
									 if (m_Context->Window->IsKey(GLFW_KEY_UP))
										 transform.Translate.y += delta;
									 if (m_Context->Window->IsKey(GLFW_KEY_DOWN))
										 transform.Translate.y -= delta;
									 if (m_Context->Window->IsKey(GLFW_KEY_KP_0))
										 transform.Translate.z -= delta;
									 if (m_Context->Window->IsKey(GLFW_KEY_KP_1))
										 transform.Translate.z += delta;
								 }*/
							 });

						 // finalize frame
						 m_Context->Renderer->EndShadowPass();
					 });
			 }
			
	};
}
