
#include "app.h"

using namespace gfx;

CApplication::CApplication()
	: m_FieldOfViewY(60.0f)				// Set the vertical view angle of the camera to 60 degrees.
	
	, m_LoopZoom(0.0f)
	, m_Count(0)

	, m_MeasureBulk(1000.0f)
	, m_Position(0.0f)

	, m_pVSBuffer(nullptr)
	, m_pPSBuffer(nullptr)

	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)

	, m_pMaterial(nullptr)
	, m_pMesh(nullptr)

	, m_ViewMatrix{ 0.0f }
	, m_ProjectionMatrix{ 0.0f }
{
}

// -----------------------------------------------------------------------------

CApplication::~CApplication()
{
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateConstantBuffers()
{
	// -----------------------------------------------------------------------------
	// Create a constant buffer with global data for the vertex shader. We use this
	// buffer to upload the data defined in the 'SVertexBuffer' struct. Note that it
	// is not possible to use the data of a constant buffer in vertex and pixel
	// shader. Constant buffers are specific to a certain shader stage. If a 
	// constant buffer is a vertex or a pixel buffer is defined in the material info
	// when creating the material.
	// -----------------------------------------------------------------------------
	CreateConstantBuffer(sizeof(VSBuffer), &m_pVSBuffer);
	CreateConstantBuffer(sizeof(PSBuffer), &m_pPSBuffer);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
	// -----------------------------------------------------------------------------
	// Important to release the buffer again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseConstantBuffer(m_pVSBuffer);
	ReleaseConstantBuffer(m_pPSBuffer);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
	// -----------------------------------------------------------------------------
	// Load and compile the shader programs.
	// -----------------------------------------------------------------------------
	CreateVertexShader("..\\data\\shader\\simple.fx", "VSMain", &m_pVertexShader);
	CreatePixelShader( "..\\data\\shader\\simple.fx", "PSMain", &m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseShader()
{
	// -----------------------------------------------------------------------------
	// Important to release the shader again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseVertexShader(m_pVertexShader);
	ReleasePixelShader(m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMaterials()
{
	// -----------------------------------------------------------------------------
	// Create a material spawning the mesh. Note that you can use the same material
	// for multiple meshes as long as the input layout of the vertex shader matches
	// the vertex layout of the mesh.
	// -----------------------------------------------------------------------------
	m_pMaterial = CreateMaterialInfo(m_pVSBuffer, m_pPSBuffer, m_pVertexShader, m_pPixelShader);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
	// -----------------------------------------------------------------------------
	// Important to release the material again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseMaterial(m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMeshes()
{
	// -----------------------------------------------------------------------------
	// Define the vertices of the mesh. We have a very simple vertex layout here,
	// because each vertex contains only its position. Take a look into the 
	// 'simple.fx' file and there into the 'VSShader'. As you can see the 'VSShader'
	// expects one argument of type 'VSInput', which is a struct containing the
	// arguments as a set of members. Note that the content of the struct matches
	// exactly the layout of each single vertex defined here.
	// -----------------------------------------------------------------------------

	m_pMesh = CreateSierpinski(m_pMaterial);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMeshes()
{
	// -----------------------------------------------------------------------------
	// Important to release the mesh again when the application is shut down.
	// -----------------------------------------------------------------------------
	ReleaseMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
	// -----------------------------------------------------------------------------
	// The projection matrix defines the size of the camera frustum. The YoshiX
	// camera has the shape of a pyramid with the eye position at the top of the
	// pyramid. The horizontal view angle is defined by the vertical view angle
	// and the ratio between window width and window height. Note that we do not
	// set the projection matrix to YoshiX. Instead we store the projection matrix
	// as a member and upload it in the 'InternOnFrame' method in a constant buffer.
	// -----------------------------------------------------------------------------
	GetProjectionMatrix(60.0f, static_cast<float>(_Width) / static_cast<float>(_Height), 0.01f, 100.0f, m_ProjectionMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
	// -----------------------------------------------------------------------------
	// Define position and orientation of the camera in the world. The result is
	// stored in the 'm_ViewMatrix' matrix and uploaded in the 'InternOnFrame'
	// method.
	// -----------------------------------------------------------------------------

	//                  Z
	//                  ^
	//                  |
	//                  |
	//                  |
	//                  |
	//                  | Quad
	//      ----------TTTTT--------------> X
	//                  ^
	//                  |
	//                  |
	//                  |
	//                  |
	//                  x Eye

	float Eye[3];
	float At[3];
	float Up[3];

	Eye[0] = 1.4f;      Eye[1] = 1.0f;      Eye[2] = -2.0f;
	At[0] = 1.4f;       At[1] = 1.0f;       At[2] = 0.0f;
	Up[0] = 0.0f;       Up[1] = 1.0f;       Up[2] = 0.0f;

	GetViewMatrix(Eye, At, Up, m_ViewMatrix);

	return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
	// -----------------------------------------------------------------------------
	// Upload the world matrix and the view projection matrix to the GPU. This has
	// to be done before drawing the mesh, though not necessarily in this method.
	// -----------------------------------------------------------------------------

	SetAlphaBlending(true);

	VSBuffer BufferVS;
	//VSPerObjectConstants PerObjectConstantsVS;
	//VSPerFrameConstants PerFrameConstantsVS;
	PSBuffer BufferPS;
	;


	GetTranslationMatrix(m_Position, m_Position, m_Position, BufferVS.m_VSWorldMatrix);
	UploadConstantBuffer(&BufferVS, m_pVSBuffer);

	MulMatrix(m_ViewMatrix, m_ProjectionMatrix, BufferVS.m_VSViewProjectionMatrix);
	UploadConstantBuffer(&BufferVS, m_pVSBuffer);

	m_Count++;
	m_LoopZoom = (m_Count < 700) ? m_LoopZoom - 1 : m_LoopZoom + 1;

	BufferPS.m_PSLoopZoom = m_LoopZoom;
	BufferPS.m_PSMeasureBulk = m_MeasureBulk;

	UploadConstantBuffer(&BufferPS, m_pPSBuffer);

	// -----------------------------------------------------------------------------
	// Draw the mesh. This will activate the shader, constant buffers, and textures
	// of the material on the GPU and render the mesh to the current render targets.
	// -----------------------------------------------------------------------------

	DrawMesh(m_pMesh);

	return true;
}

// -----------------------------------------------------------------------------

BHandle CApplication::CreateMaterialInfo(BHandle _VSBuffer, BHandle _PSBuffer, BHandle _VertexShader, BHandle _PixelShader)
{
	//-------------------------------------------------------------------
	//		Create Based Textured Material Info
	//-------------------------------------------------------------------
	SMaterialInfo Info;

	Info.m_NumberOfTextures = 0;
	Info.m_NumberOfVertexConstantBuffers = 1;
	Info.m_pVertexConstantBuffers[0] = _VSBuffer;
	Info.m_NumberOfPixelConstantBuffers = 1;
	Info.m_pPixelConstantBuffers[0] = _PSBuffer;
	Info.m_pVertexShader = _VertexShader;
	Info.m_pPixelShader = _PixelShader;
	Info.m_NumberOfInputElements = 2;
	Info.m_InputElements[0].m_Type = SInputElement::Float3;
	Info.m_InputElements[0].m_pName = "POSITION";
	Info.m_InputElements[1].m_Type = SInputElement::Float2;
	Info.m_InputElements[1].m_pName = "TEXCOORD0";

	BHandle m_pMaterial = nullptr;

	CreateMaterial(Info, &m_pMaterial);

	return m_pMaterial;
}

// -----------------------------------------------------------------------------

BHandle CApplication::CreateSierpinski(BHandle _Material)
{
	// Scheitelpunkte
	float Vertices[][3 + 2] =
	{
		// X      Y     Z         U      V
		{ -8.0f, -8.0f, 0.0f,  -2.0f, -2.0f },
		{  8.0f, -8.0f, 0.0f,   2.0f, -2.0f },
		{  8.0f,  8.0f, 0.0f,   2.0f,  2.0f },
		{ -8.0f,  8.0f, 0.0f,  -2.0f,  2.0f },
	};

	// -----------------------------------------------------------------------------
	// Define the topology of the mesh via indices. An index addresses a vertex from
	// the array above. Three indices represent one triangle. When defining the 
	// triangles of a mesh imagine that you are standing in front of the triangle 
	// and looking to the center of the triangle. If the mesh represents a closed
	// body such as a cube, your view position has to be outside of the body. Now
	// define the indices of the addressed vertices of the triangle in counter-
	// clockwise order.
	// -----------------------------------------------------------------------------

	int Indices[][3] =
	{
		{ 0, 1, 2 }, // Triangle 0
		{ 0, 2, 3 }, // Triangle 1
	};

	// -----------------------------------------------------------------------------
	// Define the mesh and its material. The material defines the look of the 
	// surface covering the mesh. Note that you pass the number of indices and not
	// the number of triangles.
	// -----------------------------------------------------------------------------
	SMeshInfo Info;

	Info.m_pVertices = &Vertices[0][0];		// Pointer to the first float of the first vertex.
	Info.m_NumberOfVertices = 4;			// The number of vertices.
	Info.m_pIndices = &Indices[0][0];		// Pointer to the first index.
	Info.m_NumberOfIndices = 6;				// The number of indices (has to be dividable by 3).
	Info.m_pMaterial = _Material;			// A handle to the material covering the mesh.

	BHandle m_pMesh = nullptr;

	CreateMesh(Info, &m_pMesh);

	return m_pMesh;
}

// -----------------------------------------------------------------------------