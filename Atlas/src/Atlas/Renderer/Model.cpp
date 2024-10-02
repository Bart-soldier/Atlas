#include "atlaspch.h"
#include "Atlas/Renderer/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Atlas
{
	struct ModelData
	{
		std::vector<Mesh> Meshes;
	};

	static ModelData s_ModelData;

	std::vector<Mesh> Model::LoadModel(const std::filesystem::path& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	
		s_ModelData.Meshes.clear();

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			ATLAS_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return s_ModelData.Meshes;
		}

		s_ModelData.Meshes.reserve(scene->mNumMeshes);

		ProcessNode(*scene->mRootNode, *scene);

		return s_ModelData.Meshes;
	}

	void Model::ProcessNode(const aiNode& node, const aiScene& scene)
	{
		for (uint32_t meshIndex = 0; meshIndex < node.mNumMeshes; meshIndex++)
		{
			aiMesh* mesh = scene.mMeshes[node.mMeshes[meshIndex]];
			s_ModelData.Meshes.push_back(ProcessMesh(*mesh, scene));
		}

		for (uint32_t childIndex = 0; childIndex < node.mNumChildren; childIndex++)
		{
			ProcessNode(*node.mChildren[childIndex], scene);
		}
	}

	Mesh Model::ProcessMesh(const aiMesh& mesh, const aiScene& scene)
	{
		uint32_t indicesPerFace = 3; // Faces will always be triangles due to aiProcess_Triangulate

		std::vector<Mesh::Vertex> vertices;
		vertices.reserve(mesh.mNumVertices);
		std::vector<uint32_t> indices;
		indices.reserve(mesh.mNumFaces * indicesPerFace);

		for (uint32_t vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++)
		{
			glm::vec3 position = { mesh.mVertices[vertexIndex].x, mesh.mVertices[vertexIndex].y, mesh.mVertices[vertexIndex].z };
			glm::vec3 normal   = { mesh.mNormals [vertexIndex].x, mesh.mNormals [vertexIndex].y, mesh.mNormals [vertexIndex].z };
			glm::vec2 texCoords = mesh.mTextureCoords[0] ? glm::vec2(mesh.mTextureCoords[0][vertexIndex].x, mesh.mTextureCoords[0][vertexIndex].y) : glm::vec2(0.0f, 0.0f);
			vertices.push_back({position, normal, texCoords});
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
		{
			aiFace& face = mesh.mFaces[faceIndex];

			// Use face.mNumIndices if engine supports none-triangulated mesh (currently not the case)
			for (uint32_t index = 0; index < indicesPerFace; index++)
			{
				indices.push_back(face.mIndices[index]);
			}
		}

		// TODO: Materials ?

		return Mesh(vertices, indices);
	}
}