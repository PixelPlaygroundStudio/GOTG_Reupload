#pragma once

#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/core/Application.h>



#ifdef WI_PLATFORM_WINDOWS

// Make the application always use the GPU

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

extern Wiwa::Application* Wiwa::CreateApplication(int argc, char** argv);

#ifdef WI_DIST
#include <Windows.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{	
	Wiwa::Log::Init();
	WI_CORE_INFO("Initialized Log!");

	auto app = Wiwa::CreateApplication(0, nullptr);
	app->Run();

	delete app;

	return 0;
}
#else
int main(int argc, char** argcv)
{
	Wiwa::Log::Init();
	WI_CORE_INFO("Initialized Log!");

	auto app = Wiwa::CreateApplication(0, nullptr);
	app->Run();

	delete app;

	return 0;
}

#endif // WI_DIST

#endif // WI_PLATFORM_WINDOWS