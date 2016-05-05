#include "pch.h"
#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

#include "DebugExport.h"

namespace dd {

BOOL CALLBACK SymCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    auto& ret = *((Symbols*)UserContext);

    Symbol sym;
    sym.name = std::string(pSymInfo->Name, pSymInfo->Name + pSymInfo->NameLen);
    sym.addr = uint32(pSymInfo->Address - pSymInfo->ModBase);
    sym.size = pSymInfo->Size;
    ret.emplace_back(sym);

    return TRUE;
}

Symbols GatherSymbols(const Config& conf)
{
    HANDLE proc = conf.target_process != nullptr ? (HANDLE)conf.target_process : ::GetCurrentProcess();
    DWORD opt = SYMOPT_DEFERRED_LOADS;
    ::SymSetOptions(opt);
    ::SymInitialize(proc, conf.additional_search_path, TRUE);

    char filter[256];
    sprintf(filter, "%s!*", conf.target_module ? conf.target_module : "*");
    Symbols ret;
    ::SymEnumSymbols(proc, 0, filter, SymCallback, &ret);
    return ret;
}

} // namespace dd
#endif // _WIN32
