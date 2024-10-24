#include "atlaspch.h"
#include "Atlas/Renderer/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Atlas
{
	void Model::LoadModel(Ref<Scene> activeScene, const std::filesystem::path& path)
	{
		Assimp::Importer importer;
		const aiScene* modelScene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

		if (!modelScene || modelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !modelScene->mRootNode)
		{
			ATLAS_CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return;
		}

		std::filesystem::path modelPath = path;
		modelPath.remove_filename();

		ProcessNode(activeScene, modelPath, *modelScene->mRootNode, *modelScene);

		importer.FreeScene();

		return;
	}

	void Model::ProcessNode(Ref<Scene> activeScene, const std::filesystem::path& modelPath, const aiNode& node, const aiScene& modelScene, Entity* parent)
	{
		Entity* nodeParent;

		if (node.mNumMeshes != 1)
		{
			nodeParent = activeScene->CreateEntity(node.mName.C_Str(), parent);

			for (uint32_t meshIndex = 0; meshIndex < node.mNumMeshes; meshIndex++)
			{
				aiMesh* mesh = modelScene.mMeshes[node.mMeshes[meshIndex]];

				Entity* meshEntity = activeScene->CreateEntity(mesh->mName.C_Str(), nodeParent);
				meshEntity->AddComponent<MeshComponent>(CreateMesh(*mesh, modelScene));
				//meshEntity->AddComponent<MaterialComponent>(CreateMaterial(*mesh, modelPath, modelScene)); TODO: Fix: Memory leak?
			}
		}
		else
		{
			aiMesh* mesh = modelScene.mMeshes[node.mMeshes[0]];

			Entity* meshEntity = activeScene->CreateEntity(mesh->mName.C_Str(), parent);
			meshEntity->AddComponent<MeshComponent>(CreateMesh(*mesh, modelScene));
			//meshEntity->AddComponent<MaterialComponent>(CreateMaterial(*mesh, modelPath, modelScene)); TODO: Fix: Memory leak?

			nodeParent = meshEntity;
		}

		for (uint32_t childIndex = 0; childIndex < node.mNumChildren; childIndex++)
		{
			ProcessNode(activeScene, modelPath, *node.mChildren[childIndex], modelScene, nodeParent);
		}
	}

	Ref<Mesh> Model::CreateMesh(const aiMesh& mesh, const aiScene& modelScene)
	{
		uint32_t indicesPerFace = 3; // Faces will always be triangles due to aiProcess_Triangulate

		std::vector<Mesh::Vertex> vertices;
		vertices.reserve(mesh.mNumVertices);
		std::vector<uint32_t> indices;
		indices.reserve(mesh.mNumFaces * indicesPerFace);

		for (uint32_t vertexIndex = 0; vertexIndex < mesh.mNumVertices; vertexIndex++)
		{
			glm::vec3 position  = { mesh.mVertices[vertexIndex].x, mesh.mVertices[vertexIndex].y, mesh.mVertices[vertexIndex].z };
			glm::vec3 normal    = { mesh.mNormals [vertexIndex].x, mesh.mNormals [vertexIndex].y, mesh.mNormals [vertexIndex].z };
			glm::vec2 texCoords = mesh.mTextureCoords[0] ? glm::vec2(mesh.mTextureCoords[0][vertexIndex].x, mesh.mTextureCoords[0][vertexIndex].y) : glm::vec2(0.0f, 0.0f);
			glm::vec3 tangent   = { mesh.mTangents[vertexIndex].x, mesh.mTangents[vertexIndex].y,  mesh.mTangents[vertexIndex].z };
			vertices.push_back({position, normal, texCoords, tangent});
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

		return CreateRef<Mesh>(vertices, indices);
	}
	Ref<Material> Model::CreateMaterial(const aiMesh& mesh, const std::filesystem::path& modelPath, const aiScene& modelScene)
	{
		Ref<Texture2D> diffuseTexture = nullptr;
		Ref<Texture2D> specularTexture = nullptr;

		if (mesh.mMaterialIndex > 0)
		{
			aiMaterial* material = modelScene.mMaterials[mesh.mMaterialIndex];
			aiString path;

			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			diffuseTexture = Texture2D::Create(modelPath / path.C_Str());
			if (!diffuseTexture->IsLoaded())
			{
				diffuseTexture = nullptr;
			}

			material->GetTexture(aiTextureType_SPECULAR, 0, &path);
			specularTexture = Texture2D::Create(modelPath / path.C_Str());
			if (!specularTexture->IsLoaded())
			{
				specularTexture = nullptr;
			}
		}

		return CreateRef<Material>(diffuseTexture, specularTexture);
	}
}