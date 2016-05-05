#include "pch.h"
#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

#include "DebugExport.h"

namespace de {

BOOL CALLBACK SymCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    auto& handler = *((const SymbolHandler*)UserContext);

    const char *name = pSymInfo->Name;
    size_t name_len = pSymInfo->NameLen;
    void *addr = (void*)(pSymInfo->Address - pSymInfo->ModBase);
    size_t size = pSymInfo->Size;
    handler(name, addr, size);

    return TRUE;
}

void GatherSymbols(const Config& conf, const SymbolHandler& sh)
{
    HANDLE proc = conf.target_process != nullptr ? (HANDLE)conf.target_process : ::GetCurrentProcess();
    DWORD opt = SYMOPT_DEFERRED_LOADS;
    ::SymSetOptions(opt);
    ::SymInitialize(proc, conf.additional_search_path, TRUE);

    char filter[256];
    sprintf(filter, "%s!*", conf.target_module ? conf.target_module : "*");
    ::SymEnumSymbols(proc, 0, filter, SymCallback, (void*)&sh);
}

} // namespace de
#endif // _WIN32
