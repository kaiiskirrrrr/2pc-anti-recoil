#ifndef RECOIL_HXX
#define RECOIL_HXX


#include <Windows.h>
#include <commctrl.h>
#include <thread>
#include <utility>

#include "device/kmbox/bpro.hxx"
#include "device/dma/communication.hxx"
#include "tools/timing.hxx"

#pragma comment(lib, "Comctl32.lib")

class recoil
{
private:
    auto update_key_information() -> void;

public:
    struct recoil_settings
    {
        bool enabled = true;
        bool key_pressed = false;

        int toggle_key = VK_F1;

        std::pair<int, int> position = { 0, 3 };
    };
    static inline recoil_settings settings{};

    auto start() -> bool
    {
        std::jthread([this] { update_key_information(); }).detach();

        limiter limiter(150);
        while (true)
        {
            limiter.start();
            performance_timer::update_tick(0);

            while (!settings.enabled)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (settings.key_pressed)
            {
                auto& communication_obj = *bpro::communication_t.get();
                communication_obj.move(settings.position.first, settings.position.second, 1);
            }

            limiter.end();
        }
    }
};
inline const auto recoil_t = std::make_unique<recoil>();

#endif // !RECOIL_HXX
