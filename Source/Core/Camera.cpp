#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
    GetCursorPos(&m_LastMousePos);
}

void Camera::Initialize(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
    Update(0.0f);
}

void Camera::Update(float dt)
{
    ProcessMouse(dt);
    ProcessKeyboard(dt);
}

void Camera::ProcessKeyboard(float dt)
{
    float currentSpeed = m_MoveSpeed * dt;

    // Apply speed boost when Shift is held
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        currentSpeed *= 4.0f;

    XMVECTOR pos = XMLoadFloat3(&m_Pos);
    XMVECTOR look = XMLoadFloat3(&m_LookDir);
    XMVECTOR right = XMLoadFloat3(&m_RightDir);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // Update position based on movement vectors
    if (GetAsyncKeyState('W') & 0x8000) pos += look * currentSpeed;
    if (GetAsyncKeyState('S') & 0x8000) pos -= look * currentSpeed;
    if (GetAsyncKeyState('D') & 0x8000) pos += right * currentSpeed;
    if (GetAsyncKeyState('A') & 0x8000) pos -= right * currentSpeed;
    if (GetAsyncKeyState('E') & 0x8000) pos += up * currentSpeed;
    if (GetAsyncKeyState('Q') & 0x8000) pos -= up * currentSpeed;

    XMStoreFloat3(&m_Pos, pos);
}

void Camera::ProcessMouse(float dt)
{
    // Handle rotation only when the Right Mouse Button is pressed
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
    {
        POINT currentPos;
        GetCursorPos(&currentPos);

        // Calculate cursor displacement
        float deltaX = static_cast<float>(currentPos.x - m_LastMousePos.x);
        float deltaY = static_cast<float>(currentPos.y - m_LastMousePos.y);

        float sens = m_MouseSensitivity * 0.001f;

        m_Yaw += deltaX * sens;
        m_Pitch += deltaY * sens;

        // Clamp Pitch to prevent the camera from flipping over
        m_Pitch = std::clamp(m_Pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);
    }

    GetCursorPos(&m_LastMousePos);

    // Create rotation matrix from Euler angles (Pitch, Yaw, Roll)
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);

    // Transform default orientation vectors by the rotation matrix
    XMVECTOR look = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotation);
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotation);

    // Normalize and store the direction vectors
    XMStoreFloat3(&m_LookDir, XMVector3Normalize(look));
    XMStoreFloat3(&m_RightDir, XMVector3Normalize(right));
}

XMMATRIX Camera::GetViewMatrix() const
{
    XMVECTOR pos = XMLoadFloat3(&m_Pos);
    XMVECTOR look = XMLoadFloat3(&m_LookDir);
    XMVECTOR up = XMLoadFloat3(&m_UpDir);

    // Generate Left-Handed View Matrix using camera position and look direction
    return XMMatrixLookToLH(pos, look, up);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    // Generate Left-Handed Perspective Projection Matrix
    return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), m_AspectRatio, m_NearZ, m_FarZ);
}