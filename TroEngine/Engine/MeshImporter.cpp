#include "MeshImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Material.h"
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

	//LoadFile("Library\\Meshes\\mesh_0.tromesh");

	//CubeMesh();

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

		//Check if the scene has a material
		std::vector<Material*> mats;
		if (scene->HasMaterials())
		{
			for (uint m = 0; m < scene->mNumMaterials; ++m)
			{
				Material* material = nullptr;
				aiMaterial* curr_mat = scene->mMaterials[m];
				aiString assimp_path;
				curr_mat->GetTexture(aiTextureType_DIFFUSE, 0, &assimp_path);
				std::string path = assimp_path.C_Str();
				uint name_pos = path.find_last_of("\\");
				
				std::string rel_path = "Models\\";
				rel_path += path.substr(name_pos + 1, path.size() - name_pos + 1);
				EDITOR_LOG("File material %d path is: %s. Start importing it...", m, rel_path.c_str());

				material = App->materials->ImportImage(rel_path.c_str());
				mats.push_back(material);
			}			
		}
		//Create base gameobject
		std::string file_path = path;
		uint name_start = file_path.find_last_of("\\");
		uint name_end = file_path.find_last_of(".");
		std::string root_go_name = file_path.substr(name_start+1,name_end-name_start-1);

		GameObject* root_go = App->scene_manager->GetCurrentScene()->CreateGameObject(root_go_name.c_str());

		//Import scene from root node
		ret = ImportNode(scene, scene->mRootNode, root_go, mats);
	}
	else
	{
		EDITOR_LOG("Error loading scene %s", path);
		ret = false;
	}

	return ret;
}

bool MeshImporter::ImportNode(const aiScene * scene, aiNode * node, GameObject * parent, const std::vector<Material*>& mats)
{
	bool ret = true;

	//Get node transformation
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	float3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	//Get node name
	aiString name = node->mName;

	//Import node meshes
	uint num_meshes = node->mNumMeshes;
	GameObject* first_go = parent; //store the first mesh go as the parent of children meshes, if the node has no meshes this node parent is the parent
	for (uint i = 0; i < num_meshes; ++i)
	{
		GameObject* go = nullptr;

		if (parent == nullptr)
		{
			go = App->scene_manager->GetCurrentScene()->CreateGameObject(name.C_Str());
		}
		else
		{
			go = parent->CreateChild(name.C_Str());
		}

		if (i == 0)
			first_go = go;

		//import mesh
		ret = ImportMesh(scene->mMeshes[node->mMeshes[i]], go, mats);

		//Check if the mesh was imported to the gameobject, if not delete the go.
		if (!go->HasComponent(Component::MeshRenderer))
		{
			//parent->ch
		}

		//set the transform to the go
		Transform* trans = (Transform*)go->GetComponent(Component::Transform);
		trans->SetTransform(pos, scale, rot);
	}

	//Import child nodes
	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		ImportNode(scene, node->mChildren[i], first_go, mats);
	}

	return ret;
}

bool MeshImporter::ImportMesh(aiMesh * mesh, GameObject * owner, const std::vector<Material*>& mats)
{
	bool ret = true;

	if (mesh == nullptr || owner == nullptr)
		ret = false;
	else
	{
		//Load mesh Vertices
		uint num_vertices = mesh->mNumVertices;
		float* vert = new float[num_vertices * 3];
		memcpy(vert, mesh->mVertices, sizeof(float) * num_vertices * 3);
		EDITOR_LOG("Mesh has %d vertices", num_vertices);

		//Load mesh Indices
		uint num_indices;
		uint* indices = nullptr;
		if (mesh->HasFaces())
		{
			num_indices = mesh->mNumFaces * 3;
			indices = new uint[num_indices]; // assume each face is a triangle
			for (uint i = 0; i < mesh->mNumFaces; ++i)
			{
				if (mesh->mFaces[i].mNumIndices != 3)
				{
					EDITOR_LOG("WARNING, geometry face with != 3 indices!");
					ret = false;
				}
				else
					memcpy(&indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
			EDITOR_LOG("Mesh has %d indices", num_indices);
		}

		//Load mesh UV
		uint num_uv = 0;
		float* uv = nullptr;
		if (mesh->HasTextureCoords(0)) // assume mesh has one texture coords
		{
			num_uv = mesh->mNumVertices;

			uv = new float[num_uv * 3];
			memcpy(uv, mesh->mTextureCoords[0], sizeof(float)*num_uv * 3);
		}
		else
		{
			EDITOR_LOG("No Texture Coords found for mesh");
			ret = false;
		}

		//Load mesh Normals
		uint num_normals = 0;
		float* normals = nullptr;
		if (mesh->HasNormals())
		{
			num_normals = mesh->mNumVertices;

			normals = new float[num_normals * 3];
			memcpy(normals, mesh->mNormals, sizeof(float)*num_normals * 3);
		}

		if (ret == false) //invalild mesh, discart 
		{
			RELEASE_ARRAY(vert);
			if (indices != nullptr)
				RELEASE_ARRAY(indices);
			if (uv != nullptr)
				RELEASE_ARRAY(uv);
			if (normals != nullptr)
				RELEASE_ARRAY(normals);

			EDITOR_LOG("Error loading mesh");
		}
		else //valid mesh, add to the list
		{
			Mesh* new_mesh = new Mesh(num_vertices, vert, num_indices, indices, num_uv, uv, num_normals, normals);
			meshes.push_back(new_mesh);

			//add a mesh renderer to owner game object
			MeshRenderer* mr = (MeshRenderer*)owner->AddComponent(Component::Type::MeshRenderer);
			//Set this mesh to the mesh renderer
			mr->SetMesh(new_mesh);

			//Set corresponding material
			ComponentMaterial* material = (ComponentMaterial*)owner->AddComponent(Component::Type::C_Material);
			material->SetMaterial(mats[mesh->mMaterialIndex]);

			//Sver to library
			SaveToLibrary(new_mesh);
		}
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
	go->AddComponent(Component::Type::Camera);

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

void MeshImporter::CubeMesh()
{
	float ver[24] =
	{
		15.f,5.f,5.f,
		10.f,5.f,5.f,
		10.f,0.f,5.f,
		15.f,0.f,5.f,
		15.f,0.f,0.f,
		15.f,5.f,0.f,
		10.f,5.f,0.f,
		10.f,0.f,0.f
	};

	uint indices[36] =
	{
		0,1,2, 2,3,0,
		0,3,4, 4,5,0,
		0,5,6, 6,1,0,
		1,6,7, 7,2,1,
		7,4,3, 3,2,7,
		4,7,6, 6,5,4 
	};

	cube = new Mesh(24, ver, 36, indices);
	meshes.push_back(cube);
}
