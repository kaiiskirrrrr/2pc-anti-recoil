#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <setupapi.h>
#include <devguid.h>
#include <thread>
#include <string_view>
#include "conio.h"

namespace bpro
{
    class connection
    {
    private:

        auto find_port(const std::string& target_description) const -> std::string
        {
            HDEVINFO device_info_set = SetupDiGetClassDevsA(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
            if (device_info_set == INVALID_HANDLE_VALUE)
                return "";

            SP_DEVINFO_DATA device_info_data;
            device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

            for (DWORD index = 0; SetupDiEnumDeviceInfo(device_info_set, index, &device_info_data); ++index)
            {
                char buffer[512];
                DWORD size = 0;

                if (SetupDiGetDeviceRegistryPropertyA(device_info_set, &device_info_data, SPDRP_FRIENDLYNAME, NULL, (PBYTE)buffer, sizeof(buffer), &size) and size > 0)
                {
                    buffer[size] = '\0';
                    std::string device_description = buffer;

                    size_t com_position = device_description.find("COM");
                    size_t end_position = device_description.find(")", com_position);

                    if (com_position != std::string::npos and end_position != std::string::npos and device_description.find(target_description) != std::string::npos)
                    {
                        std::string com_port = device_description.substr(com_position, end_position - com_position);

                        int port_number = std::stoi(com_port.substr(3));
                        if (port_number >= 10) {
                            com_port = "\\\\.\\COM" + std::to_string(port_number);
                        }

                        SetupDiDestroyDeviceInfoList(device_info_set);
                        return com_port;
                    }
                }
            }

            SetupDiDestroyDeviceInfoList(device_info_set);
            return "";
        }

        auto open_port(HANDLE& serial_handle, const char* port_name, DWORD baud_rate) const -> bool
        {
            serial_handle = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

            bool extra_flags = false;

            if (serial_handle == INVALID_HANDLE_VALUE)
                return false;

            DCB serial_params = { 0 };
            serial_params.DCBlength = sizeof(serial_params);

            if (!GetCommState(serial_handle, &serial_params))
            {
                CloseHandle(serial_handle);
                return false;
            }

            serial_params.BaudRate = baud_rate;
            serial_params.ByteSize = 8;
            serial_params.StopBits = ONESTOPBIT;
            serial_params.Parity = NOPARITY;

            if (extra_flags)
            {
                serial_params.fBinary = 1;
                serial_params.fOutxCtsFlow = 0;
                serial_params.fOutxDsrFlow = 0;
                serial_params.fRtsControl = RTS_CONTROL_DISABLE;
                serial_params.fDtrControl = DTR_CONTROL_DISABLE;
                serial_params.fDsrSensitivity = 0;

                serial_params.fTXContinueOnXoff = 0;
                serial_params.fOutX = 0;
                serial_params.fInX = 0;
                serial_params.fErrorChar = 0;
                serial_params.fNull = 0;
            }

            if (!SetCommState(serial_handle, &serial_params))
            {
                CloseHandle(serial_handle);
                return false;
            }

            COMMTIMEOUTS timeouts = { 0 };

            timeouts.ReadIntervalTimeout = 50;
            timeouts.ReadTotalTimeoutConstant = 50;
            timeouts.ReadTotalTimeoutMultiplier = 10;
            timeouts.WriteTotalTimeoutConstant = 50;
            timeouts.WriteTotalTimeoutMultiplier = 10;

            if (!SetCommTimeouts(serial_handle, &timeouts))
            {
                CloseHandle(serial_handle);
                return false;
            }

            return true;
        }

    public:

        HANDLE bpro_handle;

        auto connect_port() -> bool
        {
            const std::string port_number = this->find_port("USB-SERIAL CH340");

            if (port_number.empty())
            {
                return false;
            }

            if (this->open_port(this->bpro_handle, port_number.c_str(), 115200))
            {
                return true;
            }

            return false;
        }
    };
    inline const auto connection_t = std::make_unique<connection>();

    class communication
    {
    private:

        __forceinline auto send_command(std::string_view command) const -> void
        {
            DWORD bytes_written;
            WriteFile(connection_t->bpro_handle, command.data(), static_cast<DWORD>(command.size()), &bytes_written, 0);
        }

    public:

        __forceinline auto move(int x, int y, int beizer) -> void
        {
            constexpr auto cmd_buffer_size = 24;
            char cmd[24];

            const int command_length = std::snprintf(cmd, cmd_buffer_size, "km.move(%d, %d, %d)\r\n", x, y, beizer);
            if (command_length > 0 and static_cast<size_t>(command_length) < cmd_buffer_size)
            {
                this->send_command(cmd);
            }
        }
    };
    inline const auto communication_t = std::make_unique<communication>();

}