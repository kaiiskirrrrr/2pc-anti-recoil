#ifndef ENTRY_POINT_CXX
#define ENTRY_POINT_CXX

#include <iostream>

#include "device./kmbox/bpro.hxx"
#include "device/dma/communication.hxx"
#include "recoil.hxx"
#include "frontend/start/initialization.hxx"

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

auto throw_error(const char* error_message) -> void
{
	MessageBoxA(nullptr, error_message, "no bueno!", MB_ICONERROR | MB_OK);
	ExitProcess(EXIT_FAILURE);
}

auto __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) -> std::int32_t
{
	if (!bpro::connection_t->connect_port()) { throw_error("kmbox port not found"); }
	if (!request->prepare_modules()) { throw_error("failed to find modules"); }
	if (!request->start_handle()) { throw_error("failed to connect to dma handle"); }
	if (!keyboard_manager_t->connect()) { throw_error("failed to connect to keyboard manager"); }

	std::jthread([] { recoil_t->start(); }).detach();

	auto& rendering_obj = *n_rendering::c_rendering.get();

	rendering_obj.initialize_window();
	rendering_obj.initialize_directx();
	rendering_obj.initialize_render();
}

#endif // !ENTRY_POINT_CXX
