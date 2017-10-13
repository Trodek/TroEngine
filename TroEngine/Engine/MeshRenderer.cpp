#include "MeshRenderer.h"
#include "GameObject.h"
#include "Mesh.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(Component::Type::MeshRenderer,owner)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Draw()
{
	for (std::list<Mesh*>::iterator m = meshes.begin(); m != meshes.end(); ++m)
	{
		(*m)->Render(wireframe);
	}
}

void MeshRenderer::DebugDraw()
{
}

void MeshRenderer::CleanUp()
{
}

void MeshRenderer::AddMesh(Mesh * mesh)
{
	meshes.push_back(mesh);
}

void MeshRenderer::DrawConfig()
{
}
