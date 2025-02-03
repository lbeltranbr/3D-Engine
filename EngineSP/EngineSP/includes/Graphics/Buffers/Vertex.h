#pragma once
#include "Common/Core.h"

namespace SP {

	struct QuadVertex {
		float Data[4] = { .0f, .0f, .0f, .0f };
	};
	struct FlatVertex {
		glm::vec3 Position = glm::vec3(.0f);
		glm::vec4 Color = glm::vec4(.0f);
	};
	struct ShadedVertex {

		glm::vec3 Position = glm::vec3(.0f);
		glm::vec3 Normal = glm::vec3(.0f);
		glm::vec2 UVs = glm::vec2(.0f);

		glm::vec3 Tangent = glm::vec3(.0f);
		glm::vec3 Bitangent = glm::vec3(.0f);

	};
	struct SkyboxVertex
	{
		glm::vec3 Position = glm::vec3(0.0f);
	};

	template <typename Vertex>
	struct MeshData {
		std::vector<uint32_t> Indices;
		std::vector<Vertex> Vertices;
	};
}
