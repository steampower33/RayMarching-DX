#pragma once

class Camera;

class Constant
{
public:
    Constant() {}
    ~Constant() {}

    // [Rule] System classes should NOT be copied.
    // Copying a core system creates ambiguity in resource ownership.
    Constant(const Constant&) = delete;
    Constant& operator=(const Constant&) = delete;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void UpdateConstant(Camera& camera, float deltaTime, float totalTime, float width, float height);

public:
    void BindConstantBuffer();
    struct Constants
    {
        float iTime;
        float iResolution[2];
        float iFogDensity;    // 16 bytes

        float iCameraPos[3];
        float iConeHeight;    // 16 bytes

        float iCameraForward[3];
        float iConeRadius;    // 16 bytes

        float iCameraRight[3];
        float padding1;       // 16 bytes

        float iCameraUp[3];
        float padding2;       // 16 bytes

        float iFogColor[3];
        float padding3;       // 16 bytes

        float iSunDir[3];
        float padding4;       // 16 bytes
    };

private:
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_Context = nullptr;

private:
    void CreateConstantBuffer();
    ComPtr<ID3D11Buffer> m_ConstantBuffer;

};