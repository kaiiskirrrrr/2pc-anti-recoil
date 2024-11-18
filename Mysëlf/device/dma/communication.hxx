#pragma once

#include "pch.hxx"
#include <array>
#include <cstdint>
#include <cstdint>           
#include <cstdio>            
#include <cstdlib>            
#include <iostream>       
#include <map>
#include <memory>
#include <windows.h>       

class dma_handler 
{
private:
    struct modules 
    {
        HMODULE vmm = nullptr;
        HMODULE leech_core = nullptr;
        HMODULE ftd3xx = nullptr;
    } modules_t;

public:

    VMM_HANDLE global_handle;

    auto prepare_modules() -> bool 
    {
        modules_t.vmm = LoadLibraryA("vmm.dll");
        modules_t.leech_core = LoadLibraryA("leechcore.dll");
        modules_t.ftd3xx = LoadLibraryA("FTD3XX.dll");

        return modules_t.vmm and modules_t.leech_core and modules_t.ftd3xx;
    }

    auto start_handle() -> bool
    {
        LPCSTR args[] =
        {
            "-device",
            "fpga://algo=0",
            "-norefresh",
            "",
            "",
            "",
            ""
        };

        this->global_handle = VMMDLL_Initialize(3, args);
        return this->global_handle != nullptr; 
    }
};
inline const auto request = std::make_unique<dma_handler>();

class keyboard_manager
{
private:

    enum class e_registry_type
    {
        none = REG_NONE,
        sz = REG_SZ,
        expand_sz = REG_EXPAND_SZ,
        binary = REG_BINARY,
        dword = REG_DWORD,
        dword_little_endian = REG_DWORD_LITTLE_ENDIAN,
        dword_big_endian = REG_DWORD_BIG_ENDIAN,
        link = REG_LINK,
        multi_sz = REG_MULTI_SZ,
        resource_list = REG_RESOURCE_LIST,
        full_resource_descriptor = REG_FULL_RESOURCE_DESCRIPTOR,
        resource_requirements_list = REG_RESOURCE_REQUIREMENTS_LIST,
        qword = REG_QWORD,
        qword_little_endian = REG_QWORD_LITTLE_ENDIAN
    };

    struct key_data_struct
    {
        std::uint64_t gaf_async_key_state = 0;
        int pid = 0;
        std::uint8_t state_bitmap[64]{};
        std::uint8_t previous_state_bitmap[256 / 8]{ };

    } key_data;

    auto read_virtual_memory(int pid, UINT64 address, PVOID buffer, SIZE_T size, bool kernel_memory = false) -> bool
    {
        UINT64 AdjustedPID = kernel_memory ? pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY : pid;
        return VMMDLL_MemReadEx(request->global_handle, AdjustedPID, address, (PBYTE)buffer, size, 0, VMMDLL_FLAG_NOCACHE);;
    }

    template <typename T>
    auto read(int pid, UINT64 address, bool kernel_memory = false) -> T
    {
        T buffer;
        read_virtual_memory(pid, address, &buffer, sizeof(T), kernel_memory);
        return buffer;
    }

    auto get_pid(LPCSTR process_name) -> DWORD
    {
        DWORD pid = 0;
        VMMDLL_PidGetFromName(request->global_handle, process_name, &pid);
        return pid;
    }

    auto CC_TO_LPSTR(const char* in) -> LPSTR
    {
        LPSTR out = new char[strlen(in) + 1];
        strcpy_s(out, strlen(in) + 1, in);

        return out;
    }

    auto get_registry_value(const char* path, e_registry_type type) -> std::string 
    {
        BYTE buffer[0x128];
        DWORD _type = (DWORD)type;
        DWORD size = sizeof(buffer);

        if (!VMMDLL_WinReg_QueryValueExU(request->global_handle, CC_TO_LPSTR(path), &_type, buffer, &size))
        {
            return "failed to query mem ptr handle";
        }

        std::wstring wstr = std::wstring((wchar_t*)buffer);
        return std::string(wstr.begin(), wstr.end());
    }

    auto get_pid_list(std::string name) -> std::vector<int>
    {
        PVMMDLL_PROCESS_INFORMATION process_info = 0;
        DWORD total_processes = 0;
        std::vector<int> list = { };

        if (!VMMDLL_ProcessGetInformationAll(request->global_handle, &process_info, &total_processes))
        {
            return list;
        }

        for (size_t i = 0; i < total_processes; i++)
        {
            auto process = process_info[i];
            if (std::strstr(process.szNameLong, name.c_str()))
                list.push_back(process.dwPID);
        }

        return list;
    }

public:

    auto connect() -> bool
    {
        std::string win = this->get_registry_value("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\CurrentBuild", e_registry_type::sz);
        int Winver = 0;
        if (!win.empty())
            Winver = std::stoi(win);
        else
            return false;

        key_data.pid = this->get_pid("winlogon.exe");
        if (Winver > 22000)
        {
            auto pids = this->get_pid_list("csrss.exe");
            for (size_t i = 0; i < pids.size(); i++)
            {
                auto pid = pids[i];
                uintptr_t tmp = VMMDLL_ProcessGetModuleBaseU(request->global_handle, pid, (LPSTR)"win32ksgd.sys");
                uintptr_t g_session_global_slots = tmp + 0x3110;
                uintptr_t user_session_state = this->read<uintptr_t>(this->read<uintptr_t>(this->read<uintptr_t>(g_session_global_slots, pid), pid), pid);
                key_data.gaf_async_key_state = user_session_state + 0x3690;
                if (key_data.gaf_async_key_state > 0x7FFFFFFFFFFF)
                    break;
            }
            if (key_data.gaf_async_key_state > 0x7FFFFFFFFFFF)
                return true;
            return false;
        }
        else
        {
            PVMMDLL_MAP_EAT eat_map = NULL;
            PVMMDLL_MAP_EATENTRY eat_map_entry;
            bool result = VMMDLL_Map_GetEATU(request->global_handle, this->get_pid("winlogon.exe") | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, (LPSTR)"win32kbase.sys", &eat_map);
            if (!result)
            {
                return false;
            }

            if (eat_map->dwVersion != VMMDLL_MAP_EAT_VERSION)
            {
                VMMDLL_MemFree(eat_map);
                eat_map_entry = NULL;
                return false;
            }

            for (int i = 0; i < eat_map->cMap; i++)
            {
                eat_map_entry = eat_map->pMap + i;
                if (strcmp(eat_map_entry->uszFunction, "gafAsyncKeyState") == 0)
                {
                    key_data.gaf_async_key_state = eat_map_entry->vaFunction;
                    break;
                }
            }

            VMMDLL_MemFree(eat_map);
            eat_map = NULL;

            if (key_data.gaf_async_key_state > 0x7FFFFFFFFFFF)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    auto update_key_state() -> void
    {
       std::uint8_t previous_key_state_bitmap[64] = { 0 };
        memcpy(previous_key_state_bitmap, key_data.state_bitmap, 64);

        VMMDLL_MemReadEx(request->global_handle, this->key_data.pid | VMMDLL_PID_PROCESS_WITH_KERNELMEMORY, key_data.gaf_async_key_state, (PBYTE)&key_data.state_bitmap, 64, NULL, VMMDLL_FLAG_NOCACHE);
        for (int vk = 0; vk < 256; ++vk)
        {
            if ((key_data.state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2) and !(previous_key_state_bitmap[(vk * 2 / 8)] & 1 << vk % 4 * 2))
            {
                key_data.previous_state_bitmap[vk / 8] |= 1 << vk % 8;
            }
        }
    }

    auto key_down(std::uint32_t virtual_key) -> bool
    {
        return key_data.state_bitmap[(virtual_key * 2 / 8)] & (1 << (virtual_key % 4 * 2));
    }
};
inline const auto keyboard_manager_t = std::make_unique<keyboard_manager>();