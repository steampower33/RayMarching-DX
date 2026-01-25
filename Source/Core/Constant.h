#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <SimpleMath.h>

class Camera;

class Constant
{
public:
    // Nested type aliases to avoid global namespace pollution
    using Vector2 = DirectX::SimpleMath::Vector2;
    using Vector3 = DirectX::SimpleMath::Vector3;

public:
    struct Constants
    {
        // Register 0
        float   iTime;              // Elapsed time in seconds
        Vector2 iResolution;        // Viewport resolution (Width, Height)
        float   iStepSize;          // Raymarching step size

        // Register 1
        Vector3 iCameraPos;         // World space camera position
        float   iCloudScale;        // Noise frequency for cloud shaping

        // Register 2
        Vector3 iCameraForward;     // Camera look direction
        float   iCloudThreshold;    // Cloud coverage cutoff

        // Register 3
        Vector3 iCameraRight;       // Camera right vector
        float   iAbsorption;        // Light absorption coefficient

        // Register 4
        Vector3 iCameraUp;          // Camera up vector
        float   iFogDensity;        // Global fog density

        // Register 5
        Vector3 iSunDir;            // Direction to the sun
        float   padding1;           // 16-byte alignment padding

        // Register 6
        Vector3 iSunColor;          // RGB color of the sunlight
        float   padding2;           // 16-byte alignment padding

        // Register 7
        Vector3 iFogColor;          // RGB color of the atmospheric fog
        float   padding3;           // 16-byte alignment padding
    };

public:
    Constant() = default;
    ~Constant() = default;

    // [Rule] System classes should NOT be copied.
    Constant(const Constant&) = delete;
    Constant& operator=(const Constant&) = delete;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void UpdateConstant(Camera& camera, float deltaTime, float totalTime, float width, float height);
    void BindConstantBuffer();

    Constants m_Constants;

private:
    void CreateConstantBuffer();
    void InitData();

private:
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_Context = nullptr;
    ComPtr<ID3D11Buffer>    m_ConstantBuffer;
};