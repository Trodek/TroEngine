#include "MeshImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "MaterialManager.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "Inspector.h"

#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "AssimpLogger.h"
#include "Assimp\include\DefaultLogger.hpp"
#include "Assimp\include\Logger.hpp"

#pragma comment (lib, "Engine/Assimp/libx86/assimp.lib")

MeshImporter::MeshImporter()
{
	SetName("meshimporter");
}

MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Awake(JSONDoc * config)
{
	bool ret = true;
	return ret;
}

bool MeshImporter::Start()
{
	bool ret = true;
	Assimp::DefaultLogger::create();
	const uint severity = Assimp::Logger::Debugging | Assimp::Logger::Info | Assimp::Logger::Err | Assimp::Logger::Warn;
	Assimp::DefaultLogger::get()->attachStream(new AssimpLogger(),severity);
	return ret;
}

bool MeshImporter::CleanUp()
{
	bool ret = true;

	for (std::list<Mesh*>::iterator g = meshes.begin(); g != meshes.end();)
	{
		(*g)->CleanUp();
		RELEASE(*g);

		g = meshes.erase(g);
	}

	aiDetachAllLogStreams();

	return ret;
}

bool MeshImporter::LoadFile(const char * path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		EDITOR_LOG("%d Meshes detected. Start Loading...", scene->mNumMeshes);

		AABB* bigest_bbox = nullptr;

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			//Load vertices
			aiMesh* curr_mesh = scene->mMeshes[i];
			uint num_vertices = curr_mesh->mNumVertices;
			float* vert = new float[num_vertices * 3];
			memcpy(vert, curr_mesh->mVertices, sizeof(float) * num_vertices * 3);
			EDITOR_LOG("Mesh %d has %d vertices", i+1, num_vertices);
			
			//Load indices
			uint num_indices;
			uint* indices = nullptr;
			if (curr_mesh->HasFaces())
			{
				num_indices = curr_mesh->mNumFaces * 3;
				indices = new uint[num_indices]; // assume each face is a triangle
				for (uint i = 0; i < curr_mesh->mNumFaces; ++i)
				{
					if (curr_mesh->mFaces[i].mNumIndices != 3)
					{
						EDITOR_LOG("WARNING, geometry face with != 3 indices!");
						ret = false;
					}
					else
						memcpy(&indices[i * 3], curr_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
				EDITOR_LOG("Mesh %d has %d indices", i+1, num_indices);
			}

			//Load uv
			uint num_uv = 0;
			float* uv = nullptr;
			if (curr_mesh->HasTextureCoords(0)) // assume mesh has one texture coords
			{
				num_uv = curr_mesh->mNumVertices;
				
				uv = new float[num_uv * 3];
				memcpy(uv, curr_mesh->mTextureCoords[0], sizeof(float)*num_uv * 3);
			}
			else
			{
				EDITOR_LOG("No Texture Coords found for mesh %d", i + 1);
				ret = false;
			}

			if (ret == false) //invalild mesh, discart 
			{
				RELEASE_ARRAY(vert);
				if (indices != nullptr)
					RELEASE_ARRAY(indices);
				if (uv != nullptr)
					RELEASE_ARRAY(uv);
				EDITOR_LOG("Error loading mesh %d", i+1);
			}
			else //valid mesh, add to the list
			{
				Mesh* new_geo = new Mesh(num_vertices, vert, num_indices, indices, num_uv, uv);
				meshes.push_back(new_geo);
				//for now, add the mesh to scene game object too
				MeshRenderer* mr = (MeshRenderer*) App->scene_manager->GetCurrentScene()->GetGameObject(0)->GetComponent(Component::Type::MeshRenderer);
				mr->AddMesh(new_geo);

				//check bbox size
				if (bigest_bbox == nullptr)
					bigest_bbox = &new_geo->GetAABB();
				else
				{
					if (new_geo->GetAABB().Size().LengthSq() > bigest_bbox->Size().LengthSq())
					{
						bigest_bbox = &new_geo->GetAABB();
					}
				}
				
			}
		}
		//Open inspector to see properties
		App->gui->inspector->active = true;

		//focus camera to biggest mesh
		if(bigest_bbox != nullptr)
			App->camera->AdjustCameraToAABB(*bigest_bbox);

		//Check if has a material associated
		if (scene->HasMaterials())
		{
			aiMaterial* mat = scene->mMaterials[0]; //just one material is supported now
			aiString path;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

			std::string rel_path = "Models\\";
			rel_path += path.C_Str();

			EDITOR_LOG("Mesh material path is: %s. Start importing it...", rel_path.c_str());
			App->materials->ImportImage(rel_path.c_str());

		}

		//Get transform
		aiNode* root = scene->mRootNode;

		aiMatrix4x4 m = root->mTransformation;

		float4x4 transform(m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);

		Transform* t = (Transform*) App->scene_manager->GetCurrentScene()->GetGameObject(0)->GetComponent(Component::Type::Transform);
		t->SetTransform(transform);

		aiReleaseImport(scene);
	}
	else
	{
		EDITOR_LOG("Error loading scene %s", path);
		ret = false;
	}

	return ret;
}

void MeshImporter::RemoveMesh(Mesh * m)
{
	for (std::list<Mesh*>::iterator mesh = meshes.begin(); mesh != meshes.end();++m)
	{
		if (*mesh == m)
		{
			(*mesh)->CleanUp();
			RELEASE(*mesh);

			meshes.erase(mesh);
			break;
		}
	}
}