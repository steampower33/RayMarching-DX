#include "Camera.h"

#include "Constant.h"

using namespace DirectX;

void Constant::UpdateConstant(Camera& camera, float deltaTime, float totalTime, float width, float height)
{
	if (!m_ConstantBuffer)
	{
		OutputDebugStringA("[Error] Constant Buffer is NULL!\n");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	m_Context->Map(m_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Constants* data = (Constants*)msr.pData;

	data->iTime = totalTime;
	data->iResolution[0] = width;
	data->iResolution[1] = height;

	auto pos = camera.m_Pos;
	data->iCameraPos[0] = pos.x;
	data->iCameraPos[1] = pos.y;
	data->iCameraPos[2] = pos.z;

	auto fwd = camera.m_LookDir;
	data->iCameraForward[0] = fwd.x;
	data->iCameraForward[1] = fwd.y;
	data->iCameraForward[2] = fwd.z;

	auto right = camera.m_RightDir;
	data->iCameraRight[0] = right.x;
	data->iCameraRight[1] = right.y;
	data->iCameraRight[2] = right.z;

	auto up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMFLOAT3 upF; XMStoreFloat3(&upF, up);
	data->iCameraUp[0] = upF.x;
	data->iCameraUp[1] = upF.y;
	data->iCameraUp[2] = upF.z;

    m_Context->Unmap(m_ConstantBuffer.Get(), 0);
}

void Constant::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Device = device;
	m_Context = context;

	CreateConstantBuffer();
}

void Constant::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantbufferdesc = {};
	// Ensure size is a multiple of 16 bytes for HLSL compatibility
	constantbufferdesc.ByteWidth = sizeof(Constants);
	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // Updated every frame from CPU
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT hr = m_Device->CreateBuffer(&constantbufferdesc, nullptr, &m_ConstantBuffer);
	if (FAILED(hr))
	{
		OutputDebugStringA("[Error] Constant Buffer is NULL!\n");
	}
}

void Constant::BindConstantBuffer()
{
	m_Context->PSSetConstantBuffers(0, 1, m_ConstantBuffer.GetAddressOf());
}