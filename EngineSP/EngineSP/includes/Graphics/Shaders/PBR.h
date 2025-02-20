#pragma once
#include "Shader.h"
#include"../Utilities/Data.h"

namespace SP
{
    struct PbrShader : Shader
    {
        SP_INLINE PbrShader(const std::string& filename) : Shader(filename)
        {
            //CAMERA
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");

            //MAT PROPERTIES
            u_Roughness = glGetUniformLocation(m_ShaderID, "u_material.roughness");
            u_Occlusion = glGetUniformLocation(m_ShaderID, "u_material.occlusion");
            u_Emissive = glGetUniformLocation(m_ShaderID, "u_material.emissive");
            u_Metalness = glGetUniformLocation(m_ShaderID, "u_material.metalness");
            u_Albedo = glGetUniformLocation(m_ShaderID, "u_material.albedo");

            //MAT MAPS
            u_RoughnessMap = glGetUniformLocation(m_ShaderID, "u_material.RoughnessMap");
            u_OcclusionMap = glGetUniformLocation(m_ShaderID, "u_material.OcclusionMap");
            u_EmissiveMap = glGetUniformLocation(m_ShaderID, "u_material.EmissiveMap");
            u_MetalnessMap = glGetUniformLocation(m_ShaderID, "u_material.MetalnessMap");
            u_AlbedoMap = glGetUniformLocation(m_ShaderID, "u_material.AlbedoMap");
            u_NormalMap = glGetUniformLocation(m_ShaderID, "u_material.NormalMap");

            u_UseRoughnessMap = glGetUniformLocation(m_ShaderID, "u_material.UseRoughnessMap");
            u_UseOcclusionMap = glGetUniformLocation(m_ShaderID, "u_material.UseOcclusionMap");
            u_UseEmissiveMap = glGetUniformLocation(m_ShaderID, "u_material.UseEmissiveMap");
            u_UseMetalnessMap = glGetUniformLocation(m_ShaderID, "u_material.UseMetalnessMap");
            u_UseAlbedoMap = glGetUniformLocation(m_ShaderID, "u_material.UseAlbedoMap");
            u_UseNormalMap = glGetUniformLocation(m_ShaderID, "u_material.UseNormalMap");

            //GLOBAL ILLUMINATION
            u_IrradMap = glGetUniformLocation(m_ShaderID, "u_irradMap"); 
            u_PrefilMap = glGetUniformLocation(m_ShaderID, "u_prefilMap");
            u_BrdfMap = glGetUniformLocation(m_ShaderID, "u_brdfMap");

            //LIGHTS
            u_NbrPointLight = glGetUniformLocation(m_ShaderID, "u_nbrPointLight");
            u_NbrDirectLight = glGetUniformLocation(m_ShaderID, "u_nbrDirectLight");
            u_NbrSpotLight = glGetUniformLocation(m_ShaderID, "u_nbrSpotLight");

            //SHADOWS
            u_LightSpace = glGetUniformLocation(m_ShaderID, "u_lightSpace");
            u_DepthMap = glGetUniformLocation(m_ShaderID, "u_depthMap");
            u_ShadowBias = glGetUniformLocation(m_ShaderID, "u_shadowBias");
        }
        //-------------POINT LIGHT-------------//
        SP_INLINE void SetPointLight(PointLight& light, Transform3D& transform, int32_t index)
        {
            std::string intensity = "u_pointLights[" + std::to_string(index) + "].Intensity";
            std::string radiance = "u_pointLights[" + std::to_string(index) + "].Radiance";
            std::string position = "u_pointLights[" + std::to_string(index) + "].Position";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());
            uint32_t u_position = glGetUniformLocation(m_ShaderID, position.c_str());

            glUniform3fv(u_position, 1, &transform.Translate.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);
            glUniform1f(u_intensity, light.Intensity);
        }
        SP_INLINE void SetPointLightCount(int32_t count)
        {
            glUniform1i(u_NbrPointLight, count);
        }
        //-------------DIRECT LIGHT-------------//
        SP_INLINE void SetDirectLight(DirectLight& light, Transform3D& transform, int32_t index)
        {
            std::string intensity = "u_directLights[" + std::to_string(index) + "].Intensity";
            std::string direction = "u_directLights[" + std::to_string(index) + "].Direction";
            std::string radiance = "u_directLights[" + std::to_string(index) + "].Radiance";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_direction = glGetUniformLocation(m_ShaderID, direction.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());

            glUniform3fv(u_direction, 1, &transform.Rotation.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);
            glUniform1f(u_intensity, light.Intensity);
            glUniform1f(u_ShadowBias, light.ShadowBias);

        }
        SP_INLINE void SetDirectLightCount(int32_t count)
        {
            glUniform1i(u_NbrDirectLight, count);
        }
        //-------------SPOT LIGHT-------------//
        SP_INLINE void SetSpotLight(SpotLight& light, Transform3D& transform, int32_t index)
        {
            std::string intensity = "u_spotLights[" + std::to_string(index) + "].Intensity";
            std::string direction = "u_spotLights[" + std::to_string(index) + "].Direction";
            std::string radiance = "u_spotLights[" + std::to_string(index) + "].Radiance";
            std::string position = "u_spotLights[" + std::to_string(index) + "].Position";
            std::string falloff = "u_spotLights[" + std::to_string(index) + "].FallOff";
            std::string cutoff = "u_spotLights[" + std::to_string(index) + "].CutOff";

            uint32_t u_intensity = glGetUniformLocation(m_ShaderID, intensity.c_str());
            uint32_t u_direction = glGetUniformLocation(m_ShaderID, direction.c_str());
            uint32_t u_radiance = glGetUniformLocation(m_ShaderID, radiance.c_str());
            uint32_t u_position = glGetUniformLocation(m_ShaderID, position.c_str());
            uint32_t u_falloff = glGetUniformLocation(m_ShaderID, falloff.c_str());
            uint32_t u_cutoff = glGetUniformLocation(m_ShaderID, cutoff.c_str());

            glUniform3fv(u_direction, 1, &transform.Rotation.x);
            glUniform3fv(u_position, 1, &transform.Translate.x);
            glUniform3fv(u_radiance, 1, &light.Radiance.x);

            glUniform1f(u_falloff, glm::radians(light.FallOff));
            glUniform1f(u_cutoff, glm::radians(light.CutOff));
            glUniform1f(u_intensity, light.Intensity);
        }
        SP_INLINE void SetSpotLightCount(int32_t count)
        {
            glUniform1i(u_NbrSpotLight, count);
        }
        //-------------CAMERA-------------//
        SP_INLINE void SetCamera(Camera3D& camera, Transform3D& transform, float ratio)
        {
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio))); //1
            glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform))); //2
            glUniform3fv(u_ViewPos, 1, &transform.Translate.x);
        }
        //-------------ENVIRONMENT-------------//
        SP_INLINE void SetEnvMaps(uint32_t irrad, uint32_t prefil, uint32_t brdf, uint32_t depthMap)
        {
            // irradiance map
            glActiveTexture(GL_TEXTURE1);  // 0 is for the non used ones
            glBindTexture(GL_TEXTURE_CUBE_MAP, irrad);
            glUniform1i(u_IrradMap, 1);

            // prefiltered map
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefil);
            glUniform1i(u_PrefilMap, 2);

            // brdf map
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, brdf);
            glUniform1i(u_BrdfMap, 3);

            // Depth Map
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(u_DepthMap, 4);
        }
        SP_INLINE void SetLightSpaceMatrix(const glm::mat4& lightSpaceMtx)
        {
            // set view projection matrix
            glUniformMatrix4fv(u_LightSpace, 1, GL_FALSE, glm::value_ptr(lightSpaceMtx));
        }
        //-------------DRAW-------------//
        SP_INLINE void Draw(Model3D& model, PbrMaterial& material ,Transform3D& transform)
        {
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));

            glUniform3fv(u_Emissive, 1, &material.Emissive.x);
            glUniform3fv(u_Albedo, 1, &material.Albedo.x);
            glUniform1f(u_Roughness, material.Roughness);
            glUniform1f(u_Occlusion, material.Occlusion);
            glUniform1f(u_Metalness, material.Metalness);


            int32_t unit = 5; 

            bool useMap = false;

            // albedo map
            useMap = material.AlbedoMap != nullptr;
            glUniform1i(u_UseAlbedoMap, useMap);
            if (useMap) 
            { 
                material.AlbedoMap->Use(u_AlbedoMap, unit++);
#ifdef ENABLE_SPDLOG
#if ENABLE_COMMENTS == 1
				spdlog::info("PBR: passing AlbedoMap");
#endif
#endif
            }

            // normal map
            useMap = material.NormalMap != nullptr;
            glUniform1i(u_UseNormalMap, useMap);
            if (useMap) 
            { 
                material.NormalMap->Use(u_NormalMap, unit++);
                #ifdef ENABLE_SPDLOG
                    #if ENABLE_COMMENTS == 1
                        spdlog::info("PBR: passing NormalMap");
                    #endif
                #endif
                        int error = glGetError();

                        if (error == 0)
                            spdlog::info("normal-------");
                        if (error == GL_INVALID_ENUM)
                            spdlog::error("An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
                        if (error == GL_INVALID_VALUE)
                            spdlog::error("A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
                        if (error == GL_INVALID_OPERATION)
                            spdlog::error("The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");

            }
            // metallic map
            useMap = material.MetalnessMap != nullptr;
            glUniform1i(u_UseMetalnessMap, useMap);
            if (useMap) 
            { 
                material.MetalnessMap->Use(u_MetalnessMap, unit++);
                #ifdef ENABLE_SPDLOG
                    #if ENABLE_COMMENTS == 1
                        spdlog::info("PBR: passing MetalnessMap");
                    #endif
                #endif
            }

          
            // roughness map
            useMap = material.RoughnessMap != nullptr;
            glUniform1i(u_UseRoughnessMap, useMap);
            if (useMap) 
            { 
                material.RoughnessMap->Use(u_RoughnessMap, unit++);
                #ifdef ENABLE_SPDLOG
                    #if ENABLE_COMMENTS == 1
                        spdlog::info("PBR: passing RoughnessMap");
                    #endif
                #endif
            }

            // emissive map
            useMap = material.EmissiveMap != nullptr;
            glUniform1i(u_UseEmissiveMap, useMap);
            if (useMap) 
            { 
                material.EmissiveMap->Use(u_EmissiveMap, unit++);
                #ifdef ENABLE_SPDLOG
                    #if ENABLE_COMMENTS == 1
                        spdlog::info("PBR: passing EmissiveMap");
                    #endif
                #endif
            }

            // occlusion map
            useMap = material.OcclusionMap != nullptr;
            glUniform1i(u_UseOcclusionMap, useMap);
            if (useMap) 
            { 
                material.OcclusionMap->Use(u_OcclusionMap, unit++);
                #ifdef ENABLE_SPDLOG
                    #if ENABLE_COMMENTS == 1
                        spdlog::info("PBR: passing OcclusionMap");
                    #endif
                #endif
            }

            //draw
            model->Draw(GL_TRIANGLES);
        }
        SP_INLINE void Draw(Mesh3D& mesh, Transform3D& transform)
        {
            // set transform
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));//0
            // render mesh
            mesh->Draw(GL_TRIANGLES);
        }

    private:

        uint32_t u_Model = 0u;
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
        uint32_t u_ViewPos = 0u;

        uint32_t u_Roughness = 0u;
        uint32_t u_Occlusion = 0u;
        uint32_t u_Emissive = 0u;
        uint32_t u_Metalness = 0u;
        uint32_t u_Albedo = 0u;

        uint32_t u_RoughnessMap = 0u;
        uint32_t u_OcclusionMap = 0u;
        uint32_t u_EmissiveMap = 0u;
        uint32_t u_MetalnessMap = 0u;
        uint32_t u_AlbedoMap = 0u;
        uint32_t u_NormalMap = 0u;

        uint32_t u_UseRoughnessMap = 0u;
        uint32_t u_UseOcclusionMap = 0u;
        uint32_t u_UseEmissiveMap = 0u;
        uint32_t u_UseMetalnessMap = 0u;
        uint32_t u_UseAlbedoMap = 0u;
        uint32_t u_UseNormalMap = 0u;

        uint32_t u_NbrPointLight = 0u;
        uint32_t u_NbrDirectLight = 0u;
        uint32_t u_NbrSpotLight = 0u;

        uint32_t u_IrradMap = 0u;
        uint32_t u_PrefilMap = 0u;
        uint32_t u_BrdfMap = 0u;

        uint32_t u_LightSpace = 0u;
        uint32_t u_DepthMap = 0u;
        uint32_t u_ShadowBias = 0u;
    };
}