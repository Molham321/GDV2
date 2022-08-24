
#pragma once

#include "yoshix.h"

struct VSBuffer								// Vertex Buffer Per Object Constants
{
	float m_VSWorldMatrix[16];
	float m_VSViewProjectionMatrix[16];
};

struct PSBuffer								// Pixel Buffer Per Object Constants
{
	float	m_PSLoopZoom;
	float	m_PSMeasureBulk;
	float	m_Padding0[2];
};

class CApplication : public gfx::IApplication
{
public:

	CApplication();
	virtual ~CApplication();

private:

	using BHandle = gfx::BHandle;		// Namespase for BHandle

	BHandle m_pVSBuffer;				// A pointer to a YoshiX constant buffer, which defines global data for a vertex shader.
	BHandle m_pPSBuffer;

	BHandle m_pVertexShader;			// A pointer to a YoshiX vertex shader, which processes each single vertex of the mesh.
	BHandle m_pPixelShader;				// A pointer to a YoshiX pixel shader, which computes the color of each pixel visible of the mesh on the screen.

	BHandle m_pMaterial;				// A pointer to a YoshiX material, spawning the surface of the mesh.
	BHandle m_pMesh;					// A pointer to a YoshiX mesh, which represents a single triangle.

	float   m_Position;
	float   m_FieldOfViewY;             // Vertical view angle of the camera
	float	m_LoopZoom;
	float	m_MeasureBulk;
	float	m_Count;

	float   m_ViewMatrix[16];			// The view matrix to transform a mesh from world space into view space.
	float   m_ProjectionMatrix[16];		// The projection matrix to transform a mesh from view space into clip space.


private:

	virtual bool InternOnCreateConstantBuffers();
	virtual bool InternOnReleaseConstantBuffers();
	virtual bool InternOnCreateShader();
	virtual bool InternOnReleaseShader();
	virtual bool InternOnCreateMaterials();
	virtual bool InternOnReleaseMaterials();
	virtual bool InternOnCreateMeshes();
	virtual bool InternOnReleaseMeshes();
	virtual bool InternOnResize(int _Width, int _Height);
	virtual bool InternOnUpdate();
	virtual bool InternOnFrame();

private:

	BHandle CreateMaterialInfo(BHandle _VSBuffer, BHandle _PSBuffer, BHandle _VertexShader, BHandle _PixelShader);
	BHandle CreateSierpinski(BHandle _Material);
};

// -----------------------------------------------------------------------------