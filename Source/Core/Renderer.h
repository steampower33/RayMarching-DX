#pragma once

class Renderer
{
public:
    Renderer();
    ~Renderer();

    // [Rule] System classes should NOT be copied.
    // Copying a core system creates ambiguity in resource ownership.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void PrepareShader();
    void RenderQuad();

private:
    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_Context = nullptr;

private:
    void CreateShader();

    // Shader pipeline objects
    ComPtr<ID3D11VertexShader> m_FullScreenVS;
    ComPtr<ID3D11PixelShader> m_RayMarchingPS;
    ComPtr<ID3D11InputLayout> m_InputLayout;
    unsigned int m_Stride;

    void CreateQuadVertexBuffer();
    ComPtr<ID3D11Buffer> m_VertexBuffer;
};