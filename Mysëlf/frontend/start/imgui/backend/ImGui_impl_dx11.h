// dear ImGui: Renderer Backend for DirectX11
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'ID3D11ShaderResourceView*' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.

// You can use unmodified ImGui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire ImGui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// Learn about Dear ImGui:
// - FAQ                  https://dearImGui.com/faq
// - Getting Started      https://dearImGui.com/getting-started
// - Documentation        https://dearImGui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of ImGui.cpp

#pragma once
#include "../imgui.h"
#ifndef ImGui_DISABLE

struct ID3D11Device;
struct ID3D11DeviceContext;

ImGui_IMPL_API bool     ImGui_ImplDX11_Init(ID3D11Device* device, ID3D11DeviceContext* device_context);
ImGui_IMPL_API void     ImGui_ImplDX11_Shutdown();
ImGui_IMPL_API void     ImGui_ImplDX11_NewFrame();
ImGui_IMPL_API void     ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Dear ImGui state.
ImGui_IMPL_API void     ImGui_ImplDX11_InvalidateDeviceObjects();
ImGui_IMPL_API bool     ImGui_ImplDX11_CreateDeviceObjects();

#endif // #ifndef ImGui_DISABLE
