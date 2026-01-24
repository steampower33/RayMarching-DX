
#include "GraphicsCore.h"
#include "Renderer.h"
#include "Gui.h"

class TerraForgeApp {

public:
    TerraForgeApp() {}
    ~TerraForgeApp() {}

    // [Rule] System classes should NOT be copied.
    // Copying a core system creates ambiguity in resource ownership.
    TerraForgeApp(const TerraForgeApp&) = delete;
    TerraForgeApp& operator=(const TerraForgeApp&) = delete;

    GraphicsCore m_Gfx;
    Renderer m_Renderer;
    Gui m_Gui;

    void Initialize(HINSTANCE hInstance);

    bool Run();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    float m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f,};
};