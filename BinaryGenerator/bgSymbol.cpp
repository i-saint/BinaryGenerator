#include "pch.h"
#include "bgInternal.h"

namespace bg {

SymbolTable::SymbolTable(Context *ctx)
    : m_ctx(ctx)
{
}

SymbolTable::SymbolTable(Context *ctx, const SymbolTable& base)
    : m_ctx(ctx)
    , m_symbols(base.m_symbols)
{
}

SymbolTable::Symbols& SymbolTable::getSymbols()
{
    return m_symbols;
}

Symbol& SymbolTable::addSymbol(const Symbol& sym)
{
    if (Symbol *i = findSymbol(m_ctx->getString(sym.name))) {
        return *i;
    }

    m_symbols.emplace_back(sym);
    m_symbols.back().index = (uint32)m_symbols.size() - 1;
    return m_symbols.back();
}

Symbol* SymbolTable::getSymbol(size_t i)
{
    if (i > m_symbols.size()) { return nullptr; }
    return &m_symbols[i];
}

Symbol* SymbolTable::findSymbol(const char *name)
{
    if (!name) { return nullptr; }
    for (auto& s : m_symbols) {
        if (strcmp(m_ctx->getString(s.name), name) == 0) {
            return &s;
        }
    }
    return nullptr;
}

uint32 SymbolTable::getVirtualAddress(const char *name)
{
    Symbol *sym = findSymbol(name);
    if (!sym) { return 0; }

    uint32 ret = sym->addr;
    if (sym->section != nullsection) {
        ret += m_ctx->getSection(sym->section)->getVirtualAddress();
    }
    return ret;
}

} // namespace bg
