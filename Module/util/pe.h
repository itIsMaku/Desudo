#pragma once
#include <Windows.h>
#include <optional>

namespace util
{
    namespace detail
    {
        struct SegmentInfo
        {
            uintptr_t baseAddress;
            uintptr_t size;
        };
    }

    IMAGE_DOS_HEADER* get_dos_header(uintptr_t imageBase);
    IMAGE_NT_HEADERS* get_nt_headers(uintptr_t imageBase);

    std::optional<detail::SegmentInfo> get_module_segment(const char* moduleName, const char* segmentName);
}