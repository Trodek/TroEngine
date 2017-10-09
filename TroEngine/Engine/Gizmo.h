#ifndef __GIZMO__
#define __GIZMO__

#include "Globals.h"
#include "GLInclude.h"
#include <vector>

class Gizmo
{
public:
	Gizmo()
	{}
	~Gizmo()
	{}

	virtual void Start()
	{}
	virtual void Render()
	{	}

public:
	int mem_id = 0;
};

// ---------------------------

class GCubeVA : public Gizmo
{
public:
	GCubeVA() {}
	~GCubeVA() {}

	void Start();
	void Render();
};

// ---------------------------

class GCubeIE : public Gizmo
{
public:
	GCubeIE() {}
	~GCubeIE() {}

	void Start();
	void Render();

private:
	int indices_mem = 0;
};

// ---------------------------

class GSphere : public Gizmo
{
public:
	GSphere(){}
	~GSphere(){}

	void Start(float radius, uint rings, uint sectors);
	void Render();

private:
	float radius;
	uint rings;
	uint sectors;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLushort> indices;

};

// ---------------------------

class GCylinder : public Gizmo
{
public:
	GCylinder() {}
	~GCylinder() {}

	void Start(int slices, float r,float h);
	void Render();

private:
	std::vector<float> vertices;
};

// ---------------------------

class GArrow : public Gizmo
{
public:
	GArrow();
	~GArrow();

	void Start();
	void Render();

};

// ---------------------------

class GAxis : public Gizmo
{
public:
	GAxis();
	~GAxis();

	void Start();
	void Render();

};

// ---------------------------

class GRay : public Gizmo
{
public:
	GRay();
	~GRay();

	void Start();
	void Render();

};

// ---------------------------

class GPlane : public Gizmo
{
public:
	GPlane();
	~GPlane();

	void Start();
	void Render();

};

// ---------------------------

class GCapsule : public Gizmo
{
public:
	GCapsule();
	~GCapsule();
	
	void Start();
	void Render();

};

// ---------------------------

class GFrustum : public Gizmo
{
public:
	GFrustum();
	~GFrustum();

	void Start();
	void Render();

};
#endif // !__GIZMO__

