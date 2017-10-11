#include "MeshImporter.h"
#include "Globals.h"
#include "Mesh.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"

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
		
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

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
		EDITOR_LOG("%d Meshes detected. Start Loading...", scene->mNumMeshes)
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
				uv = new float[num_uv * 2];
				memcpy(uv, curr_mesh->mTextureCoords[0], sizeof(float)*num_uv * 2);

			}
			else
			{
				EDITOR_LOG("No Texture Coords found for mesh %d", i + 1);
				ret = false;
			}

			if (ret == false) //invalild mesh, discart 
			{
				RELEASE_ARRAY(vert);
				if (indices != NULL)
					RELEASE_ARRAY(indices);
				EDITOR_LOG("Error loading mesh %d", i);
			}
			else //valid mesh, add to the list
			{
				Mesh* new_geo = new Mesh(num_vertices, vert, num_indices, indices, num_uv, uv);
				meshes.push_back(new_geo);
			}
		}
		
		aiReleaseImport(scene);
	}
	else
	{
		EDITOR_LOG("Error loading scene %s", path);
		ret = false;
	}

	return ret;
}

void MeshImporter::RenderMeshes() const
{
	for (std::list<Mesh*>::const_iterator g = meshes.begin(); g != meshes.end(); ++g)
	{
		(*g)->Render();
	}
}
