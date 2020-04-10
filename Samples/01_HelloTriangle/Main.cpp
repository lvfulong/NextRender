#include "Platform.h"

extern std::unique_ptr<Application> CreateApplication();

#include "Platform/Windows/WindowsPlatform.h"
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	WindowsPlatform platform{hInstance, hPrevInstance, lpCmdLine, nCmdShow};
	auto app = CreateApplication();
	app->SetName("Hello Triangle");

	if (platform.Initialize(std::move(app)))
	{
		platform.MainLoop();
		platform.Terminate();
	}
	return EXIT_SUCCESS;
}
