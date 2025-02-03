#pragma once
#include <../assimp/include/assimp/postprocess.h>
#include <../assimp/include/assimp/quaternion.h>
#include <../assimp/include/assimp/Importer.hpp>
#include <../assimp/include/assimp/scene.h>
#include "Helper.h"


namespace SP
{
	struct Model
	{
		SP_INLINE Model() = default;

		SP_INLINE Model(const std::string& filename)
		{
			Load(filename);
		}

		SP_INLINE void Load(const std::string& filename)
		{
			Assimp::Importer importer;
			uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
				aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
				aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
				aiProcess_GenUVCoords | aiProcess_FlipUVs;

			const aiScene* ai_scene = importer.ReadFile(filename, flags);

			if (!ai_scene || ai_scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
#ifdef ENABLE_SPDLOG
				spdlog::error("failed to load model: {}", importer.GetErrorString());
#endif
				return;
			}
			#ifdef ENABLE_SPDLOG
				#if ENABLE_COMMENTS == 1
							spdlog::info("****************");
							spdlog::info(filename);
							spdlog::info("****************");
				#endif
			#endif
			// parse all meshes
			ParseNode(ai_scene, ai_scene->mRootNode);
		}

		SP_INLINE void Draw(uint32_t mode)
		{
			for (auto& mesh : m_Meshes)
			{
				mesh->Draw(mode);
			}
		}

	private:
		SP_INLINE void ParseNode(const aiScene* ai_scene, aiNode* ai_node)
		{
			for (uint32_t i = 0; i < ai_node->mNumMeshes; i++)
			{
				ParseMesh(ai_scene->mMeshes[ai_node->mMeshes[i]]);
			}

			for (uint32_t i = 0; i < ai_node->mNumChildren; i++)
			{
				ParseNode(ai_scene, ai_node->mChildren[i]);
			}
		}

		SP_INLINE void ParseMesh(aiMesh* ai_mesh)
		{
			MeshData<ShadedVertex> data;

			// vertices
			for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++)
			{
				ShadedVertex vertex;

				// positions
				vertex.Position = AssimpToVec3(ai_mesh->mVertices[i]);

				// normals
				vertex.Normal = AssimpToVec3(ai_mesh->mNormals[i]);

				// texcoords
				vertex.UVs.x = ai_mesh->mTextureCoords[0][i].x;
				vertex.UVs.y = ai_mesh->mTextureCoords[0][i].y;

				vertex.Tangent = glm::normalize(AssimpToVec3(ai_mesh->mTangents[i]));
				vertex.Bitangent = glm::normalize(AssimpToVec3(ai_mesh->mBitangents[i]));

				// push to array
				data.Vertices.push_back(vertex);
			}

			// indices	
			for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++)
			{
				for (uint32_t k = 0; k < ai_mesh->mFaces[i].mNumIndices; k++)
				{
					data.Indices.push_back(ai_mesh->mFaces[i].mIndices[k]);
				}
			}

			// create new mesh instance
			auto mesh = std::make_unique<ShadedMesh>(data);
			m_Meshes.push_back(std::move(mesh));
		}

	private:
		std::vector<Mesh3D> m_Meshes;
	};

	// 3d model typedef
	using Model3D = std::shared_ptr<Model>;
}