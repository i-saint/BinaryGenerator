#pragma once

#include <string>
#include <vector>
#include "../BinaryGenerator.h"

namespace dd {

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef void* process_t;

struct Symbol
{
    std::string name;
    uint32 addr;
    uint32 size;
};
typedef std::vector<Symbol> Symbols;

struct Config
{
    process_t target_process; // null == current process
    const char* target_module; // null == main module
    const char* additional_search_path; // can be null
    bool include_static_symbol;
};

// path: path to dll, exe, so, etc
bool LoadImage(const char *path);
Symbols GatherSymbols(const Config& conf);
void WriteObj(bg::IOutputStream& os, Symbols& syms, bg::Format fmt);

} // namespace dd
