#pragma once

#include <string>
#include <vector>
#include "BinaryGenerator/BinaryGenerator.h"

namespace de {

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
typedef void* process_t;

struct Symbol
{
    uint32 name;
    uint32 addr;
    uint32 size;
};
typedef std::vector<Symbol> Symbols;

struct Config
{
    process_t target_process; // null == current process
    const char* target_module; // null == main module
    const char* additional_search_path; // can be null
};

typedef std::function<void(const char *name, void *addr, size_t size)> SymbolHandler;

// path: path to dll, exe, so, etc
bool LoadImage(const char *path);
void GatherSymbols(const Config& conf, const SymbolHandler& sh);


class Context
{
public:
    Context();
    ~Context();

    void readSymbols(const char *in_path);

    void dumpSymbols(const char *out_path);
    void generateObj(const char *out_path, bg::Format fmt, bg::Architecture arch);
    void generateDLL(const char *out_path, bg::Format fmt, bg::Architecture arch);
    void modifyExecutable(const char *target_path, bool backup = true);

    size_t getNumSymbols() const;
    Symbol* getSymbol();
    const char* getName(const Symbol& sym) const;

public:
    uint32 addName(const char *name);
    uint32 addSymbol(const Symbol& sym);

private:
    std::string makeExportDirective() const;
    void generateObj_COFF_x86(const char *out_path) const;
    void generateObj_COFF_x64(const char *out_path) const;

private:
    std::string m_string_table;
    Symbols m_syms;
};

} // namespace de
