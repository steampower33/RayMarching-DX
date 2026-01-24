#include "TerraForgeApp.h"

TerraForgeApp::TerraForgeApp()
{
}

TerraForgeApp::~TerraForgeApp()
{

}

bool TerraForgeApp::Run()
{
	bool bIsExit = false;

	while (bIsExit == false)
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT) bIsExit = true;

			m_Gfx.BeginFrame(m_ClearColor);
			m_Renderer.PrepareShader();
			m_Renderer.RenderQuad();

			m_Gfx.EndFrame();
		}

		if (bIsExit) break;
	}

	return true;
}

LRESULT CALLBACK TerraForgeApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void TerraForgeApp::Initialize(HINSTANCE hInstance)
{
	WCHAR WindowClass[] = L"TerraForge-DX";
	WCHAR Title[] = L"TerraForge-DX (DX11)";

	WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };
	RegisterClassW(&wndclass);

	float width = 1280.0f;
	float height = 720.0f;
	HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
		nullptr, nullptr, hInstance, nullptr);

	m_Gfx.Initialize(hWnd, width, height);
	m_Renderer.Initialize(m_Gfx.GetDevice(), m_Gfx.GetContext());
}
