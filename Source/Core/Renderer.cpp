#include "Vertex.h"

#include "Renderer.h"

void Renderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Device = device;
	m_Context = context;

	CreateShader();
	//CreateQuadVertexBuffer();
}

void Renderer::CreateShader()
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	if (SUCCEEDED(CompileShader(L"FullScreenVS.hlsl", "vs_5_0", &vsBlob)))
	{
		m_Device->CreateVertexShader(
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			nullptr,
			&m_FullScreenVS
		);

		//// Define Input Layout
		//D3D11_INPUT_ELEMENT_DESC layout[] =
		//{
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//};

		//m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &m_InputLayout);

		//m_Stride = sizeof(Vertex);
	}

	if (SUCCEEDED(CompileShader(L"Distance2DPS.hlsl", "ps_5_0", &psBlob)))
	{
		m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_Distance2DPS);
		psBlob->Release();
		psBlob = nullptr;
	}
	if (SUCCEEDED(CompileShader(L"Distance3DPS.hlsl", "ps_5_0", &psBlob)))
	{
		m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_Distance3DPS);
		psBlob->Release();
		psBlob = nullptr;
	}

	if (SUCCEEDED(CompileShader(L"CloudPS.hlsl", "ps_5_0", &psBlob)))
	{
		m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_CloudPS);
		psBlob->Release();
		psBlob = nullptr;
	}

	if (vsBlob) vsBlob->Release();
}

void Renderer::PrepareShader()
{
	m_Context->VSSetShader(m_FullScreenVS.Get(), nullptr, 0);

	if (m_Scene.bDistance2D)
		m_Context->PSSetShader(m_Distance2DPS.Get(), nullptr, 0);
	if (m_Scene.bDistance3D)
		m_Context->PSSetShader(m_Distance3DPS.Get(), nullptr, 0);
	if (m_Scene.bCloud)
		m_Context->PSSetShader(m_CloudPS.Get(), nullptr, 0);
	//m_Context->IASetInputLayout(m_InputLayout.Get());
}

void Renderer::Render()
{
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
	m_Context->Draw(3, 0);
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

HRESULT Renderer::CompileShader(const std::wstring& filename, const std::string& profile, ID3DBlob** shaderBlob)
{
	ID3DBlob* errorBlob = nullptr;

	std::wstring path = L"Shaders/" + filename;

	HRESULT hr = D3DCompileFromFile(
		path.c_str(),
		nullptr, 
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",                 // Entry Point
		profile.c_str(),        // version
		0, 0,
		shaderBlob,             // result
		&errorBlob
	);

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return hr;
	}

	if (errorBlob) errorBlob->Release();
	return hr;
}