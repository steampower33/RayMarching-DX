#pragma once

class Renderer
{
public:
    Renderer() {}
    ~Renderer() {}

    // [Rule] System classes should NOT be copied.
    // Copying a core system creates ambiguity in resource ownership.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void PrepareShader();
    void Render();

    struct Scene {
        bool bDistance2D = false;
        bool bDistance3D = false;
        bool bCloud = true;
    } m_Scene;

private:
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_Context = nullptr;

private:
    void CreateShader();
    ComPtr<ID3D11VertexShader> m_FullScreenVS;
    ComPtr<ID3D11PixelShader> m_Distance2DPS;
    ComPtr<ID3D11PixelShader> m_Distance3DPS;
    ComPtr<ID3D11PixelShader> m_CloudPS;
    ComPtr<ID3D11InputLayout> m_InputLayout;
    unsigned int m_Stride;

    void CreateQuadVertexBuffer();
    ComPtr<ID3D11Buffer> m_VertexBuffer;

    HRESULT CompileShader(const std::wstring& filename, const std::string& profile, ID3DBlob** shaderBlob);
};