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
#include "ResourceManager.h"

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

	CubeMesh();
	PlaneMesh();

	return ret;
}

bool MeshImporter::CleanUp()
{
	bool ret = true;

	for (std::vector<Mesh*>::iterator g = meshes.begin(); g != meshes.end();)
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

		//Crete an entry on resources
		uint key = App->resources->CreateResource(path);

		//Import scene from root node
		ret = ImportNode(scene, scene->mRootNode, root_go, mats, key, path);
	}
	else
	{
		EDITOR_LOG("Error loading scene %s", path);
		ret = false;
	}

	return ret;
}

bool MeshImporter::ImportMesh(const char * path, uint mesh_id, Resource * res)
{
	bool ret = true;

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		aiMesh* mesh = scene->mMeshes[mesh_id];

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
			Mesh* new_mesh = new Mesh(res->UID, num_vertices, vert, num_indices, indices, num_uv, uv, num_normals, normals);
			meshes.push_back(new_mesh);
			res->manager_id = meshes.size() - 1;

			SaveToLibrary(new_mesh, App->resources->BuildLibraryPath(res).c_str());
		}
	}
	return ret;
}

bool MeshImporter::ImportNode(const aiScene* scene, aiNode* node, GameObject* parent, const std::vector<Material*>& mats, uint res_key, const char* path)
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

	GameObject* first_go = parent;
	for (uint i = 0; i < num_meshes; ++i)
	{
		//create the Resource to store it
		Resource* res = App->resources->ExistResource(App->resources->GetNameFromPath(path), node->mMeshes[i]);
		bool new_res = false;
		if (res == nullptr)
		{
			res = new Resource();
			res->name = App->resources->GetNameFromPath(path);
			res->extension = App->resources->GetExtensionFromPath(path);
			res->rel_path = App->resources->GetRelativePathFromPath(path);
			res->file_id = node->mMeshes[i];
			res->type = ResourceType::R_MESH;
			new_res = true;
		}

		GameObject* go;
		if (parent == nullptr)
			go = new GameObject(res->name.c_str());
		else
			go = parent->CreateChild(name.C_Str());

		if (i == 0)
			first_go = go;

		//import mesh if needed

		if (new_res)
		{
			if (ret = ImportMesh(scene->mMeshes[node->mMeshes[i]], go, mats, res))
				App->resources->resources[res_key].push_back(res);
			else
				RELEASE(res);
		}
		else
		{
			//mesh is already loaded, set that one
			MeshRenderer* mr = (MeshRenderer*)go->AddComponent(Component::Type::C_MeshRenderer);
			mr->SetMesh(meshes[res->manager_id]);

			//Set corresponding material
			ComponentMaterial* material = (ComponentMaterial*)go->AddComponent(Component::Type::C_Material);
			material->SetMaterial(mats[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex]);
		}

		//set the transform to the go
		Transform* trans = (Transform*)go->GetComponent(Component::C_Transform);
		trans->SetTransform(pos, scale, rot);

	}

	//Import child nodes
	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		ImportNode(scene, node->mChildren[i], first_go, mats, res_key, path);
	}

	return ret;
}

bool MeshImporter::ImportMesh(aiMesh* mesh, GameObject* owner, const std::vector<Material*>& mats, Resource* res)
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
			res->manager_id = meshes.size() - 1;
			res->UID = new_mesh->GetUID();

			//add a mesh renderer to owner game object
			MeshRenderer* mr = (MeshRenderer*)owner->AddComponent(Component::Type::C_MeshRenderer);
			//Set this mesh to the mesh renderer
			mr->SetMesh(new_mesh);
			
			//Set corresponding material
			ComponentMaterial* material = (ComponentMaterial*)owner->AddComponent(Component::Type::C_Material);
			material->SetMaterial(mats[mesh->mMaterialIndex]);

			//Save to library
			SaveToLibrary(new_mesh, res);
		}
	}

	return ret;
}

void MeshImporter::SaveToLibrary(Mesh * mesh, Resource* res)
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

	res->lib_name = App->resources->GetNameFromPath(file);
	res->lib_path = App->resources->GetRelativePathFromPath(file);

	FILE* f = fopen(file, "wb");
	fwrite(data, sizeof(char), total_size, f);
	fclose(f);

	RELEASE_ARRAY(data);

	EDITOR_LOG("Created %s.", file);
}

void MeshImporter::SaveToLibrary(Mesh * mesh, const char * path)
{
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
	FILE* f = fopen(path, "wb");
	fwrite(data, sizeof(char), total_size, f);
	fclose(f);

	RELEASE_ARRAY(data);

	EDITOR_LOG("Created %s.", path);
}

bool MeshImporter::LoadFile(const char * path, Resource* res)
{
	bool ret = true;
	//Open file path and get size
	FILE* file = fopen(path, "rb");
	if (file != nullptr)
	{
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
		if (res != nullptr)
		{
			Mesh* geo = new Mesh(res->UID, elements_num[1] / 3, vert, elements_num[0], ind, elements_num[2] / 3, tex_coords);
			meshes.push_back(geo);
			res->manager_id = meshes.size() - 1;
		}
		else
		{
			Mesh* geo = new Mesh(elements_num[1] / 3, vert, elements_num[0], ind, elements_num[2] / 3, tex_coords);
			meshes.push_back(geo);
		}
		RELEASE_ARRAY(data);
	}
	else
	{
		ret = false;
	}
	
	return ret;
}

void MeshImporter::RemoveMesh(Mesh * m)
{
	for (std::vector<Mesh*>::iterator mesh = meshes.begin(); mesh != meshes.end();++m)
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
	uint ind[6 * 6] =
	{
		2, 7, 6, 2, 3, 7, // front
		11, 9, 10, 11, 8, 9, // right
		1, 4, 0, 1, 5, 4, // back
		15, 13, 12, 15, 14, 13, // left
		1, 3, 2, 1, 0, 3, // top
		7, 5, 6, 7, 4, 5  // bottom
	};

	uint num_indices = 6 * 6;
	uint bytes = sizeof(uint) * num_indices;
	uint* indices = new uint[num_indices];
	memcpy(indices, ind, bytes);

	//  vertices ------------------------
	float vert[16 * 3] =
	{
		0.5f,  0.5f,  0.5f, // 0
		-0.5f,  0.5f,  0.5f, // 1
		-0.5f,  0.5f, -0.5f, // 2
		0.5f,  0.5f, -0.5f, // 3

		0.5f, -0.5f,  0.5f, // 4
		-0.5f, -0.5f,  0.5f, // 5
		-0.5f, -0.5f, -0.5f, // 6
		0.5f, -0.5f, -0.5f, // 7

		0.5f,  0.5f,  0.5f,  // 8
		0.5f, -0.5f,  0.5f,  // 9
		0.5f, -0.5f, -0.5f,  //10
		0.5f,  0.5f, -0.5f,  //11

		-0.5f,  0.5f,  0.5f,  //12
		-0.5f, -0.5f,  0.5f,  //13
		-0.5f, -0.5f, -0.5f,  //14
		-0.5f,  0.5f, -0.5f,  //15
	};

	uint num_vertices = 16;
	bytes = sizeof(float) * num_vertices * 3;
	float* vertices = new float[num_vertices * 3];
	memcpy(vertices, vert, bytes);

	// Load texture coords
	float texture_coords[16 * 3] =
	{
		1.f,  1.f,  0.f,
		0.f,  1.f,  0.f,
		0.f,  0.f,  0.f,
		1.f,  0.f,  0.f,

		1.f,  0.f,  0.f,
		0.f,  0.f,  0.f,
		0.f,  1.f,  0.f,
		1.f,  1.f,  0.f,

		1.f,  1.f,  0.f,
		0.f,  1.f,  0.f,
		0.f,  0.f,  0.f,
		1.f,  0.f,  0.f,

		0.f,  1.f,  0.f,
		1.f,  1.f,  0.f,
		1.f,  0.f,  0.f,
		0.f,  0.f,  0.f,
	};

	float* text_coords = new float[num_vertices * 3];
	memcpy(text_coords, texture_coords, bytes);

	//create the mesh
	cube = new Mesh(num_vertices, vertices, num_indices, indices, num_vertices, text_coords);
	meshes.push_back(cube);

}

//All primitives are extracted from: http://wiki.unity3d.com/index.php/ProceduralPrimitives

void MeshImporter::PlaneMesh()
{
	float length = 1.f;
	float width = 1.f;
	int resX = 2; // 2 minimum
	int resZ = 2;

	//vertices
	uint num_vert = resX*resZ;
	float3 ver[4];
	for (int z = 0; z < resZ; z++)
	{
		// [ -length / 2, length / 2 ]
		float zPos = ((float)z / (resZ - 1) - .5f) * length;
		for (int x = 0; x < resX; x++)
		{
			// [ -width / 2, width / 2 ]
			float xPos = ((float)x / (resX - 1) - .5f) * width;
			ver[x + z * resX] = float3(xPos, 0.f, zPos);
		}
	}

	float* vertices = new float[num_vert * 3];
	for (int i = 0; i < num_vert; ++i)
	{
		memcpy(vertices + i * 3, ver[i].ptr(), sizeof(float) * 3);
	}

	//indices
	uint num_indices = (resX - 1) * (resZ - 1) * 6;
	uint ind[6];
	int t = 0;
	for (int face = 0; face < num_indices / 6; ++face)
	{
		int i = face % (resX - 1) + (face / (resZ - 1) * resX);

		ind[t++] = i + resX;
		ind[t++] = i + 1;
		ind[t++] = i;
		
		ind[t++] = i + resX;
		ind[t++] = i + resX + 1;
		ind[t++] = i + 1;
	}
	uint* indices = new uint[num_indices];
	memcpy(indices, ind, sizeof(uint)*num_indices);

	//uv
	float3 uvs[4];
	for (int v = 0; v < resZ; v++)
	{
		for (int u = 0; u < resX; u++)
		{
			uvs[u + v * resX] = float3((float)u / (resX - 1), (float)v / (resZ - 1),0.f);
		}
	}

	float* uv = new float[num_vert * 3];
	for (int i = 0; i < num_vert; ++i)
	{
		memcpy(uv + i * 3, uvs[i].ptr(), sizeof(float) * 3);
	}

	//create mesh
	plane = new Mesh(num_vert, vertices, num_indices, indices, num_vert, uv);
	meshes.push_back(plane);
}
