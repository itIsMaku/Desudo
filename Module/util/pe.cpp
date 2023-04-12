#include "pe.h"

#include <Windows.h>

#define RVA(base, rva)((ULONG_PTR)base + rva)

IMAGE_DOS_HEADER* util::get_dos_header(uintptr_t imageBase)
{
    auto dosHeader = (IMAGE_DOS_HEADER*)imageBase;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return nullptr;

    return dosHeader;
}

IMAGE_NT_HEADERS* util::get_nt_headers(uintptr_t imageBase)
{
    auto dosHeader = get_dos_header(imageBase);
    if (!dosHeader)
        return nullptr;

    auto nt_headers = (IMAGE_NT_HEADERS*)RVA(imageBase, dosHeader->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
        return nullptr;

    return nt_headers;
}

std::optional<util::detail::SegmentInfo> util::get_module_segment(const char* moduleName, const char* segmentName)
{
    HANDLE module = GetModuleHandle(moduleName);
    if (!module)
        return {};

    auto ntHeaders = get_nt_headers((uintptr_t)module);
    if (!ntHeaders)
        return {};

    auto sections = IMAGE_FIRST_SECTION(ntHeaders);
    for (auto i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++)
    {
        const char* name = (const char*)sections[i].Name;
        if (strcmp(name, segmentName) == 0)
        {
            detail::SegmentInfo info;
            info.baseAddress = (uintptr_t)RVA(module, sections[i].VirtualAddress);
            info.size = sections[i].SizeOfRawData;

            return info;
        }
    }

    return {};
}