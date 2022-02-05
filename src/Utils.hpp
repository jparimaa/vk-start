#pragma once

#include <cstdint>

// clang-format off
#define CHECK(f)                                               \
{                                                              \
    if (!(f))                                                  \
    {                                                          \
        printf("Abort. Error at %s:%d\n", __FILE__, __LINE__); \
        printf("%s\n", #f);                                    \
        abort();                                               \
    }                                                          \
}
// clang-format on

template<typename T>
uint32_t ui32Size(const T& container)
{
    return static_cast<uint32_t>(container.size());
}