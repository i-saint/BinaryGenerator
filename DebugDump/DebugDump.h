#pragma once

#include <string>
#include <vector>

namespace dd {

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef void* process_t;

struct Symbol
{
    std::string name;
    uint32 addr;
    uint32 size;
};

struct Config
{
    process_t target_process; // nullptr == current process
    const char* target_module; // nullptr == main module
    bool include_static_symbol;
};

// path: path to dll, exe, so, etc
bool LoadImage(const char *path);
std::vector<Symbol> Dump(const Config& conf);

} // namespace dd
