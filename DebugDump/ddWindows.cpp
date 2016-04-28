#include "pch.h"
#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

#include "DebugDump.h"

namespace dd {

BOOL CALLBACK SymCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    auto& ret = *((std::vector<Symbol>*)UserContext);

    return TRUE;

}

std::vector<Symbol> Dump_Win(const Config& conf)
{
    DWORD opt = SYMOPT_DEFERRED_LOADS;
    ::SymSetOptions(opt);
    ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);


    std::vector<Symbol> ret;

    HANDLE proc = conf.target_process != nullptr ? (HANDLE)conf.target_process : ::GetCurrentProcess();
    ::SymEnumSymbols(proc, 0, "*!*", SymCallback, &ret);
    return ret;
}

} // namespace dd
#endif // _WIN32
