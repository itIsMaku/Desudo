#include <iostream>
#include <filesystem>

#include <Windows.h>
#include <TlHelp32.h>

DWORD find_fivem()
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snap, &pe))
    {
        do
        {
            if (std::wstring(pe.szExeFile).find(L"GTAProcess") != std::wstring::npos)
            {
                CloseHandle(snap);
                return pe.th32ProcessID;
            }
        } while (Process32Next(snap, &pe));
    }

    CloseHandle(snap);
    return 0;
}

void inject(DWORD pid, const std::string& path)
{
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!process)
    {
        std::cout << "Failed to open process.\n";
        return;
    }

    void* buffer = VirtualAllocEx(process, nullptr, path.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!buffer)
    {
        std::cout << "Failed to create DLL path buffer in target process.\n";
        return;
    }

    if (!WriteProcessMemory(process, buffer, path.c_str(), path.size(), nullptr))
    {
        VirtualFreeEx(process, buffer, 0, MEM_RELEASE);
        CloseHandle(process);

        std::cout << "Failed to write DLL path to the buffer in target process.\n";
        return;
    }

    HANDLE thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, buffer, 0, nullptr);
    WaitForSingleObject(thread, INFINITE);

    DWORD threadResult = 0;
    GetExitCodeThread(thread, &threadResult);

    if (threadResult != 0)
        std::cout << "Successfully injected module into the target process.\n";
    else
        std::cout << "Failed to inject into the target process.\n";
    
    VirtualFreeEx(process, buffer, 0, MEM_RELEASE);
    CloseHandle(process);
}

int main()
{
    auto dll = std::filesystem::current_path() / "test.dll";
    if (!std::filesystem::exists(dll))
    {
        std::cout << "Please rebuild the module and try again.";
        return 0;
    }

    DWORD pid = find_fivem();
    if (pid != 0)
        inject(pid, dll.string().c_str());
    else
        std::cout << "Please open FiveM before trying to inject.\n";
    
    std::cin.ignore();
    return 0;
}