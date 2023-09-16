#include <Wiwa/core/GameAssemblyHandle.h>

extern "C" {
	__declspec(dllexport) GameAssemblyHandle* getAssemblyHandle();
}