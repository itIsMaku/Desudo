#include "ui/hook/d3d11_hook.h"

#include "fx/event.h"

#include "config/config_manager.h"

#include "include/xorstr/xorstr.hpp"    

#include <filesystem>

std::string g_dllPath(MAX_PATH, ' ');

void set_dll_path()
{
    HMODULE hModule = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)set_dll_path, &hModule);

    char path[MAX_PATH] = { NULL };
    GetModuleFileName(hModule, path, sizeof(path));

    // .parent_path().string() for test builds.
    g_dllPath = std::filesystem::path(path).string();
}

void start()
{
    auto d3d11Hook = ui_hook::D3D11Hook::get_instance();
    d3d11Hook.start_hook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        set_dll_path();
        start();

        // CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)start, nullptr, 0, nullptr);
    }

    return TRUE;
}