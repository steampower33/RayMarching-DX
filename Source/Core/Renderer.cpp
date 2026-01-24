#include "Vertex.h"

#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{

}

void Renderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Device = device;
	m_Context = context;

	CreateShader();
	CreateQuadVertexBuffer();
}


void Renderer::CreateShader()
{
	ID3DBlob* vertexshaderCSO;
	ID3DBlob* pixelshaderCSO;
	ID3DBlob* errorBlob;

	// NOTE: Ensure the file path exists. "./Shaders/Shader.hlsl" assumes a "Shaders" folder next to the exe.
	// Compile Vertex Shader
	HRESULT hr = D3DCompileFromFile(L"./Shaders/FullScreenVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexshaderCSO, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		return;
	}

	m_Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &m_FullScreenVS);

	// Compile Pixel Shader
	hr = D3DCompileFromFile(L"./Shaders/RayMarchingPS.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelshaderCSO, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		return;
	}

	m_Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &m_RayMarchingPS);

	// Define Input Layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &m_InputLayout);

	m_Stride = sizeof(Vertex);

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
	if (errorBlob) errorBlob->Release();
}

void Renderer::PrepareShader()
{
	m_Context->VSSetShader(m_FullScreenVS.Get(), nullptr, 0);
	m_Context->PSSetShader(m_RayMarchingPS.Get(), nullptr, 0);
	m_Context->IASetInputLayout(m_InputLayout.Get());

}

void Renderer::RenderQuad()
{
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_Stride, &offset);
	// Draw 6 vertices (2 triangles)
	m_Context->Draw(6, 0);
}


void Renderer::CreateQuadVertexBuffer()
{
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = sizeof(quad_vertices);
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // Static data
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { quad_vertices };

	m_Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &m_VertexBuffer);
}