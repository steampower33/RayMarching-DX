#include "TerraForgeApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	TerraForgeApp terraFrogeApp;
	terraFrogeApp.Initialize(hInstance);

	return terraFrogeApp.Run();
}