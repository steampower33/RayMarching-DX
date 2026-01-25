#pragma once

class Camera
{
public:
    Camera();

    void Initialize(float aspectRatio);

    void Update(float dt);

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

    DirectX::XMFLOAT3 m_Pos{ 0.0f, 3.0f, -5.0f };
    DirectX::XMFLOAT3 m_LookDir{ 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 m_UpDir{ 0.0f, 1.0f, 0.0f };
    DirectX::XMFLOAT3 m_RightDir{ 1.0f, 0.0f, 0.0f };

private:
    void ProcessKeyboard(float dt);
    void ProcessMouse(float dt);

private:
    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;

    float m_AspectRatio = 1.777f;
    float m_Fov = 90.0f;
    float m_NearZ = 0.1f;
    float m_FarZ = 1000.0f;

    float m_MoveSpeed = 10.0f;
    float m_MouseSensitivity = 5.0f;

    POINT m_LastMousePos{ 0, 0 };
};