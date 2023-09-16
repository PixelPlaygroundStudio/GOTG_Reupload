#include "assembly.h"

#include <Wiwa/utilities/Reflection.h>

#include "Components.h"
#include "Systems.h"
#include "Functions.h"

const size_t TYPE_COUNT = __COUNTER__;

GameAssemblyHandle* getAssemblyHandle()
{
    TYPELIST type_list = GET_TYPES();

    GameAssemblyHandle* handle = new GameAssemblyHandle();

    for (size_t i = 0; i < type_list->Size(); i++) {
        handle->types.push_back(type_list->at(i));
    }

    return handle;
}