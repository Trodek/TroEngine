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
#include "ComponentMaterial.h"
#include <direct.h>

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

	App->CreateFolder("Assets\\Meshes");
	App->CreateFolder("Library\\Meshes");

	return ret;
}

bool MeshImporter::Start()
{
	bool ret = true;

	Assimp::DefaultLogger::create();
	const uint severity = Assimp::Logger::Debugging | Assimp::Logger::Info | Assimp::Logger::Err | Assimp::Logger::Warn;
	Assimp::DefaultLogger::get()->attachStream(new AssimpLogger(),severity);

	LoadFile("Library\\Meshes\\mesh_0.tromesh");

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

bool MeshImporter::ImportFile(const char * path)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		EDITOR_LOG("%d Meshes detected. Start Loading...", scene->mNumMeshes);

		AABB* bigest_bbox = nullptr;

		GameObject* root_go = App->scene_manager->GetCurrentScene()->CreateGameObject();
		bool meshes_as_go = false;
		//check num meshes
		if (scene->mNumMeshes > 1)
		{
			meshes_as_go = true;
		}

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

				//Save Mesh to Library for faster loading
				SaveToLibrary(new_geo);

				//add the mesh to game object
				if (meshes_as_go)
				{
					char name[20];
					sprintf_s(name, "Mesh %d", i);
					GameObject* child = root_go->CreateChild(name);
					
					MeshRenderer* mr = (MeshRenderer*)child->AddComponent(Component::Type::MeshRenderer);
					mr->SetMesh(new_geo);
				}
				else
				{
					MeshRenderer* mr = (MeshRenderer*)root_go->AddComponent(Component::Type::MeshRenderer);
					mr->SetMesh(new_geo);
				}

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
			Material* material = App->materials->ImportImage(rel_path.c_str());

			//if material is loaded, add it to game object
			if (material != nullptr)
			{
				if (meshes_as_go)
				{
					for (int c = 0; c < root_go->GetNumChilds(); ++c)
					{
						GameObject* child = root_go->GetChild(c);
						ComponentMaterial* cm = (ComponentMaterial*)child->AddComponent(Component::Type::C_Material);
						cm->SetMaterial(material);
					}
				}
				else
				{
					ComponentMaterial* cm = (ComponentMaterial*)root_go->AddComponent(Component::Type::C_Material);
					cm->SetMaterial(material);
				}
			}
		}

		//Get transform
		aiNode* root = scene->mRootNode;

		aiVector3D translation;
		aiVector3D scaling;
		aiQuaternion rotation;
		root->mTransformation.Decompose(scaling, rotation, translation);
		float3 pos(translation.x, translation.y, translation.z);
		float3 scale(scaling.x, scaling.y, scaling.z);
		Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

		Transform* t = (Transform*) App->scene_manager->GetCurrentScene()->GetGameObject(0)->GetComponent(Component::Type::Transform);
		t->SetTransform(pos,scale,rot);

		aiReleaseImport(scene);
	}
	else
	{
		EDITOR_LOG("Error loading scene %s", path);
		ret = false;
	}

	return ret;
}

void MeshImporter::SaveToLibrary(Mesh * mesh)
{
	// Save Mesh to file
	// file structure has: 
	// amount of indices / vertices / texture_coords
	// indices / vertices / texture_coords data

	uint elements_num[3] = { mesh->GetIndicesNum(), mesh->GetVerticesNum() * 3, mesh->GetUVNum() * 3 };
	uint total_size = sizeof(elements_num) + sizeof(uint)*mesh->GetIndicesNum() + sizeof(float)*(mesh->GetVerticesNum() * 3 + mesh->GetUVNum() * 3);

	char* data = new char[total_size];
	char* cursor = data;

	//store the number of elements of each type
	uint size = sizeof(elements_num);
	memcpy(cursor, elements_num, size);
	cursor += size;

	//store indices
	size = sizeof(uint)*elements_num[0];
	memcpy(cursor, mesh->GetIndices(), size);
	cursor += size;

	//store vertices
	size = sizeof(float)*elements_num[1];
	memcpy(cursor, mesh->GetVertices(), size);
	cursor += size;

	//store texture_coords
	size = sizeof(float)*elements_num[2];
	memcpy(cursor, mesh->GetUV(), size);
	cursor += size;

	//Serialize data to file
	char file[69];
	sprintf_s(file, "Library\\Meshes\\mesh_%d.tromesh",save_id++);

	FILE* f = fopen(file, "wb");
	fwrite(data, sizeof(char), total_size, f);
	fclose(f);

	RELEASE_ARRAY(data);

	EDITOR_LOG("Created %s.", file);
}

void MeshImporter::LoadFile(const char * path)
{
	//Open file path and get size
	FILE* file = fopen(path, "rb");
	fseek(file, 0L, SEEK_END);
	uint total_size = ftell(file);
	rewind(file); //go back to file begining

	// Copy file to a data buffer
	char* data = new char[total_size];
	char* cursor = data;
	fread(data, sizeof(char), total_size, file);
	fclose(file);

	//Start reading num_elements for indices/vertices/texture_coords
	uint elements_num[3];
	uint size = sizeof(elements_num);
	memcpy(elements_num, cursor, size);
	cursor += size;

	//Read indices
	uint* ind = new uint[elements_num[0]];
	size = sizeof(uint)*elements_num[0];
	memcpy(ind, cursor, size);
	cursor += size;

	//Read vertices
	float* vert = new float[elements_num[1]];
	size = sizeof(float)*elements_num[1];
	memcpy(vert, cursor, size);
	cursor += size;

	//Read texture coords
	float* tex_coords = new float[elements_num[2]];
	size = sizeof(float)*elements_num[2];
	memcpy(tex_coords, cursor, size);
	cursor += size;

	//Create a mesh with this info
	Mesh* geo = new Mesh(elements_num[1] / 3, vert, elements_num[0], ind, elements_num[2] / 3, tex_coords);
	meshes.push_back(geo);

	//TEST
	GameObject* go = App->scene_manager->GetCurrentScene()->CreateGameObject();
	MeshRenderer* mr = (MeshRenderer*)go->AddComponent(Component::Type::MeshRenderer);
	mr->SetMesh(geo);

	RELEASE_ARRAY(data);
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