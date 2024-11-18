#include "recoil.hxx"

#include <chrono>
#include <iostream>
#include <thread>
#include <Windows.h>

auto recoil::update_key_information() -> void
{
    auto& keyboard_manager_obj = *keyboard_manager_t;

    while (true)
    {
        keyboard_manager_obj.update_key_state();

        if (keyboard_manager_obj.key_down(this->settings.toggle_key))
        {
            this->settings.enabled = !this->settings.enabled;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        this->settings.key_pressed = keyboard_manager_obj.key_down(VK_RBUTTON) and keyboard_manager_obj.key_down(VK_LBUTTON);

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}
