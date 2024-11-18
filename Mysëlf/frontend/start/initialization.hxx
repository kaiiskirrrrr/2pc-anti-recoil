#pragma once

#include <d3d11.h>
#include <dwmapi.h>
#include <wrl/client.h>
#include <memory>

#include "frontend/includes.hxx"

#include "bytes.hxx"
#include "initialization.hxx"
#include <d3dx11tex.h>
#include "frontend/menu.hxx"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace n_rendering
{
	inline HWND global_window_handle = nullptr;
	inline ID3D11Device* directx_device = nullptr;
	inline ID3D11DeviceContext* directx_device_context = nullptr;
	inline IDXGISwapChain* swap_chain = nullptr;
	inline ID3D11RenderTargetView* render_target_view = nullptr;

	class rendering
	{
	private:

		const ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

		const float clear_color_with_alpha[4] =
		{
			clear_color.x * clear_color.w,
			clear_color.y * clear_color.w,
			clear_color.z * clear_color.w,
			clear_color.w
		};

		static LRESULT WINAPI window_proc_handler(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
		{
			if (ImGui_ImplWin32_WndProcHandler(window_handle, message, w_param, l_param))
				return true;

			switch (message)
			{
			case WM_SIZE:
				if (directx_device != NULL and w_param != SIZE_MINIMIZED)
				{
				}
				return 0;
			case WM_SYSCOMMAND:
				if ((w_param & 0xfff0) == SC_KEYMENU)
					return 0;
				break;
			case WM_DESTROY:
				::PostQuitMessage(0);
				return 0;
			}
			return ::DefWindowProc(window_handle, message, w_param, l_param);
		}

	public:

		auto initialize_directx() const -> bool
		{
			DXGI_SWAP_CHAIN_DESC swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
			swap_chain_description.BufferCount = 2;
			swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.OutputWindow = global_window_handle;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Windowed = 1;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL feature_level;
			const D3D_FEATURE_LEVEL feature_levels[1] = { D3D_FEATURE_LEVEL_11_0 };

			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_description, &swap_chain, &directx_device, &feature_level, &directx_device_context);
			ID3D11Texture2D* back_buffer;

			swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

			auto status = directx_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);

			back_buffer->Release();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			ImGui_ImplWin32_Init(global_window_handle);
			ImGui_ImplDX11_Init(directx_device, directx_device_context);

			D3DX11_IMAGE_LOAD_INFO image_load_info; ID3DX11ThreadPump* thread_pump{ nullptr };
			D3DX11CreateShaderResourceViewFromMemory(directx_device, girl_begging_bytes, sizeof(girl_begging_bytes), &image_load_info, thread_pump, &byte_loading::girl_begging, NULL);

			ImFontConfig font_config;

			byte_loading::font = io.Fonts->AddFontFromMemoryTTF(font, sizeof(font), 18, &font_config, io.Fonts->GetGlyphRangesCyrillic());

			directx_device->Release();

			return true;
		}

		auto initialize_render() const -> bool
		{
			static RECT window_rect;
			MSG window_message = { NULL };
			ZeroMemory(&window_message, sizeof(MSG));

			while (window_message.message != WM_QUIT)
			{
				while (PeekMessage(&window_message, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&window_message);
					DispatchMessage(&window_message);
				}

				auto& gui_io = ImGui::GetIO();
				gui_io.DeltaTime = 1.0f / 240.0f;

				static POINT previous_cursor_position = { 0 };
				POINT current_cursor_position;
				GetCursorPos(&current_cursor_position);
				if (current_cursor_position.x != previous_cursor_position.x or current_cursor_position.y != previous_cursor_position.y)
				{
					gui_io.MousePos.x = current_cursor_position.x;
					gui_io.MousePos.y = current_cursor_position.y;
					previous_cursor_position = current_cursor_position;
				}

				static bool previous_button_state = false;
				bool current_button_state = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
				if (current_button_state != previous_button_state)
				{
					gui_io.MouseDown[0] = current_button_state;
					if (current_button_state)
					{
						gui_io.MouseClicked[0] = true;
						gui_io.MouseClickedPos[0] = gui_io.MousePos;
					}
					previous_button_state = current_button_state;
				}

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();

				ImGui::NewFrame();
				{
					c_interface_colors->set_style_ekus();

					ImGui::PushFont(byte_loading::font);
					{
						c_interface->draw_menu(global_window_handle);
					}
					ImGui::PopFont();

					ImGui::Render();
					directx_device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
					directx_device_context->ClearRenderTargetView(render_target_view, this->clear_color_with_alpha);

					ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
					swap_chain->Present(1, NULL);
				}
			}

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			DestroyWindow(global_window_handle);

			return true;
		}

		auto initialize_window() const -> bool
		{
			WNDCLASSEXW window_class;
			window_class.cbSize = sizeof(WNDCLASSEXW);
			window_class.style = CS_CLASSDC;
			window_class.lpfnWndProc = this->window_proc_handler;
			window_class.cbClsExtra = NULL;
			window_class.cbWndExtra = NULL;
			window_class.hInstance = nullptr;
			window_class.hIcon = LoadIcon(0, IDI_APPLICATION);
			window_class.hCursor = LoadCursor(0, IDC_ARROW);
			window_class.hbrBackground = nullptr;
			window_class.lpszMenuName = (L"senpai");
			window_class.lpszClassName = (L"senpai");
			window_class.hIconSm = LoadIcon(0, IDI_APPLICATION);

			RegisterClassExW(&window_class);
			global_window_handle = CreateWindowExW(
				WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
				window_class.lpszClassName,
				(L"senpai"),
				WS_POPUP,
				0, 0,
				GetSystemMetrics(SM_CXSCREEN),
				GetSystemMetrics(SM_CYSCREEN),
				0, 0,
				0, 0
			);

			SetLayeredWindowAttributes(global_window_handle, RGB(0, 0, 0), 0, LWA_COLORKEY);

			ShowWindow(global_window_handle, SW_SHOW);
			UpdateWindow(global_window_handle);

			return true;
		}

	}; inline const auto c_rendering = std::make_unique<rendering>();
}