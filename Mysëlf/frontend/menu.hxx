#pragma once

#include "includes.hxx"
#include "start/bytes.hxx"
#include "start/imgui/backend/ImGui_impl_dx11.h"
#include "start/imgui/backend/ImGui_impl_win32.h"
#include "start/imgui/imgui.h"
#include <basetsd.h>
#include <cstdlib>
#include <memory>
#include <processthreadsapi.h>
#include <recoil.hxx>
#include <string>
#include <windef.h>
#include <wingdi.h>
#include <WinUser.h>

class interface_colors
{
public:
	ImColor outline_color = ImColor(0, 0, 0, 255); // 197

	ImVec4 widget_filling = ImVec4(0.528f, 0.551f, 0.961f, 1.000f);
	ImVec4 background = ImVec4(0.478f, 0.498f, 0.866f, 1.000f);
	ImVec4 widget_outline = ImVec4(0.784f, 0.784f, 0.784f, 1.000f);
	ImVec4 accent = ImVec4(1.000f, 0.955f, 0.861f, 0.883f);
	ImColor accent_main_buttons = ImColor(141, 147, 255, 255);

	void set_style_ekus()
	{
		auto& style = ImGui::GetStyle();

		constexpr auto rounding = 2;

		style.AntiAliasedLines = true;
		style.WindowRounding = rounding;
		style.ChildRounding = rounding;
		style.FrameRounding = rounding;
		style.PopupRounding = rounding;
		style.ScrollbarRounding = rounding;
		style.GrabRounding = rounding;
		style.FrameBorderSize = 1.0f;
		style.Colors[ImGuiCol_WindowBg] = background;
		style.Colors[ImGuiCol_Border] = widget_outline;
		style.Colors[ImGuiCol_ChildBg] = background;
		style.Colors[ImGuiCol_FrameBg] = widget_filling;
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.571f, 0.594f, 1.000f, 1.000f);
		style.Colors[ImGuiCol_FrameBgActive] = widget_filling;
		style.Colors[ImGuiCol_TitleBg] = widget_filling;
		style.Colors[ImGuiCol_TitleBgActive] = widget_outline;
		style.Colors[ImGuiCol_TitleBgCollapsed] = widget_filling;
		style.Colors[ImGuiCol_MenuBarBg] = widget_filling;
		style.Colors[ImGuiCol_ScrollbarBg] = background;
		style.Colors[ImGuiCol_ScrollbarGrab] = widget_outline;
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = widget_filling;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = widget_outline;
		style.Colors[ImGuiCol_CheckMark] = accent;
		style.Colors[ImGuiCol_SliderGrab] = accent;
		style.Colors[ImGuiCol_SliderGrabActive] = accent;

		style.Colors[ImGuiCol_Button] = widget_filling;
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.571f, 0.594f, 1.000f, 1.000f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.571f, 0.594f, 1.000f, 1.000f);

		style.Colors[ImGuiCol_Header] = widget_outline;
		style.Colors[ImGuiCol_HeaderHovered] = accent;
		style.Colors[ImGuiCol_HeaderActive] = widget_outline;
		style.Colors[ImGuiCol_Separator] = accent;
		style.Colors[ImGuiCol_SeparatorHovered] = accent;
		style.Colors[ImGuiCol_SeparatorActive] = widget_outline;
		style.Colors[ImGuiCol_ResizeGrip] = widget_outline;
		style.Colors[ImGuiCol_ResizeGripHovered] = widget_filling;
		style.Colors[ImGuiCol_ResizeGripActive] = accent;
		style.Colors[ImGuiCol_PlotLines] = widget_outline;
		style.Colors[ImGuiCol_PlotLinesHovered] = accent;
		style.Colors[ImGuiCol_PlotHistogram] = widget_outline;
		style.Colors[ImGuiCol_PlotHistogramHovered] = widget_filling;
		style.Colors[ImGuiCol_TextSelectedBg] = accent;
		style.Colors[ImGuiCol_DragDropTarget] = widget_outline;
		style.Colors[ImGuiCol_NavHighlight] = widget_outline;
		style.Colors[ImGuiCol_NavWindowingHighlight] = widget_filling;
		style.Colors[ImGuiCol_NavWindowingDimBg] = background;
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.7f);
	}


}; inline const auto c_interface_colors = std::make_unique<interface_colors>();

class senpai_interface
{
public:

	const ImVec2 widget_child_size{ 650, 260 }; // 260
	const ImVec2 widget_position{ 5.8f, 5.8f };
	std::string status_message;

	auto outlined_text(const std::string& text) const -> void
	{
		this->outlined_text(text, ImVec4(1, 1, 1, 1.f), ImGui::GetCursorPos());
	}

	auto outlined_text(const std::string& text, const ImColor& color) const -> void
	{
		this->outlined_text(text, color, ImGui::GetCursorPos());
	}

	auto outlined_text(const std::string& text, const ImColor& color, const ImVec2& position) const -> void
	{
		const ImVec2 offset_array[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

		for (const auto& offset : offset_array)
		{
			ImGui::SetCursorPos(ImVec2(position.x + offset.x, position.y + offset.y));
			ImGui::TextColored(c_interface_colors.get()->outline_color, text.c_str());
		}

		ImGui::SetCursorPos(position);
		ImGui::TextColored(color, text.c_str());
	}

	auto outlined_button(const char* label, const ImVec2& button_size) -> bool
	{
		bool clicked = ImGui::Button(label, button_size);

		const auto draw_list = ImGui::GetWindowDrawList();
		ImVec2 text_pos = ImGui::GetItemRectMin();
		ImVec2 text_size = ImGui::CalcTextSize(label);

		text_pos.x += (button_size.x - text_size.x) * 0.5f;
		text_pos.y += (button_size.y - text_size.y) * 0.5f;

		draw_list->AddText(ImVec2(text_pos.x - 1, text_pos.y), IM_COL32(0, 0, 0, 125), label);
		draw_list->AddText(ImVec2(text_pos.x + 1, text_pos.y), IM_COL32(0, 0, 0, 125), label);
		draw_list->AddText(ImVec2(text_pos.x, text_pos.y - 1), IM_COL32(0, 0, 0, 125), label);
		draw_list->AddText(ImVec2(text_pos.x, text_pos.y + 1), IM_COL32(0, 0, 0, 125), label);

		draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);

		return clicked;
	}

	auto draw_menu(const HWND& hwnd) -> void
	{
		constexpr auto flags = ImGuiWindowFlags_NoDecoration;
		ImGui::Begin("senpai_cc", nullptr, flags);
		{
			static bool flip = false;
			bool is_hovered = true;

			if (is_hovered != flip)
			{
				LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
				style = is_hovered ? (style & ~WS_EX_TRANSPARENT) : (style | WS_EX_TRANSPARENT);

				SetWindowLongPtr(hwnd, GWL_EXSTYLE, style);
				SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
				UpdateWindow(hwnd);

				flip = is_hovered;
			}

			const auto draw_list = ImGui::GetWindowDrawList();
			auto& interface_colors_obj = *c_interface_colors.get();

			ImGui::SetWindowSize(widget_child_size);
			{
				auto window_position = ImGui::GetWindowPos();

				draw_list->AddRectFilled(window_position, ImVec2(500000, window_position.y + 30), interface_colors_obj.accent_main_buttons);

				draw_list->AddText(ImVec2(window_position.x + 4, window_position.y + 7), interface_colors_obj.outline_color, "Myself : Rainbow Six Siege");
				draw_list->AddText(ImVec2(window_position.x + 6, window_position.y + 7), interface_colors_obj.outline_color, "Myself : Rainbow Six Siege");
				draw_list->AddText(ImVec2(window_position.x + 5, window_position.y + 6), interface_colors_obj.outline_color, "Myself : Rainbow Six Siege");
				draw_list->AddText(ImVec2(window_position.x + 5, window_position.y + 8), interface_colors_obj.outline_color, "Myself : Rainbow Six Siege");

				draw_list->AddText(ImVec2(window_position.x + 5, window_position.y + 7), ImColor(255, 255, 255, 255), "Myself : Rainbow Six Siege");
			}

			ImGui::SetCursorPos({ widget_child_size.x - 36, 5 });
			{
				const ImVec2 button_size = ImVec2(30, 20);
				const ImVec2 button_pos = ImGui::GetCursorScreenPos();
				const ImColor text_color = ImColor(255, 255, 255, 255);
				const char* text = "X";
				const ImVec2 text_size = ImGui::CalcTextSize(text);
				const ImVec2 text_pos = ImVec2(button_pos.x + (button_size.x - text_size.x) * 0.5f, button_pos.y + (button_size.y - text_size.y) * 0.5f + 1);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("##hidden", button_size))
				{
					ImGui_ImplDX11_Shutdown();
					ImGui_ImplWin32_Shutdown();
					ImGui::DestroyContext();
					ExitProcess(EXIT_SUCCESS);
				}
				ImGui::PopStyleColor();

				const ImVec2 offset_array[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

				for (const auto& offset : offset_array)
				{
					draw_list->AddText(ImVec2(text_pos.x + offset.x, text_pos.y + offset.y), interface_colors_obj.outline_color, text);
				}

				draw_list->AddText(text_pos, text_color, text);
			}

			ImGui::SetCursorPosY(35);

			static const ImVec2 image_position = ImVec2(201, 219);
			ImGui::Image(byte_loading::girl_begging, image_position, ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));
			ImGui::SameLine();

			ImGui::BeginGroup();
			{
				auto& recoil_obj = *recoil_t.get();
				ImGui::Checkbox("enable", &recoil_obj.settings.enabled);

				ImGui::Spacing();

				ImGui::SetNextItemWidth(200);
				ImGui::SliderInt("vertical", &recoil_obj.settings.position.second, 0, 10, "%.1dpx");

				ImGui::SetNextItemWidth(200);
				ImGui::SliderInt("horizontal", &recoil_obj.settings.position.first, -10, 10, "%.1dpx");

				ImGui::Spacing();

				const auto button_text = "toggle keybind: " + key_code_to_text(recoil_obj.settings.toggle_key);
				if (outlined_button(button_text.c_str(), ImVec2(300, 26)))
				{
					bool selected = false;
					while (!selected)
					{
						for (int i = 0; i <= 0xA5; i++)
						{
							if (GetAsyncKeyState(i) & 1)
							{
								recoil_obj.settings.toggle_key = i;
								selected = true;
							}
						}
					}
				}
			}
			ImGui::EndGroup();
		}
		ImGui::End();
	}

}; inline const auto c_interface = std::make_unique<senpai_interface>();
