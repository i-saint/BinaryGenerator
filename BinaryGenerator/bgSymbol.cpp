#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"

namespace bg {

Symbol::Symbol()
    : section(), index(), addr(), flags(), name()
{
}

Symbol::Symbol(Section *s, uint32_t a, const String& n, uint32_t f)
    : section(s), index(), addr(a), flags(f), name(n)
{
}

SymbolTable::SymbolTable(Context *ctx)
    : m_ctx(ctx)
{
}

SymbolTable::Symbols& SymbolTable::getSymbols()
{
    return m_symbols;
}

Symbol& SymbolTable::addSymbol(const Symbol& sym)
{
    if (Symbol *i = findSymbol(sym.name.str())) {
        return *i;
    }

    m_symbols.emplace_back(sym);
    m_symbols.back().index = (uint32_t)m_symbols.size() - 1;
    return m_symbols.back();
}

Symbol* SymbolTable::getSymbol(size_t i)
{
    if (i > m_symbols.size()) { return nullptr; }
    return &m_symbols[i];
}

Symbol* SymbolTable::findSymbol(const char *name)
{
    for (auto& s : m_symbols) {
        if (s.name == name) {
            return &s;
        }
    }
    return nullptr;
}

} // namespace bg
