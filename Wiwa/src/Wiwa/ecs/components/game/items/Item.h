#pragma once
#pragma warning(disable : 4302)
#pragma warning(disable : 4311)
#include <Wiwa/utilities/Reflection.h>
#include <Wiwa/core/Core.h>

namespace Wiwa
{
    struct WI_API Item
    {
        char Name[128];
        uint8_t item_type;
    };
}
REFLECTION_BEGIN(Wiwa::Item)
    REFLECT_MEMBER(Name)
    REFLECT_MEMBER(item_type)
REFLECTION_END;
