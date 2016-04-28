#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"

namespace bg {

Symbol::Symbol()
    : section(), index(), rva(), flags(), name()
{
}

Symbol::Symbol(Section *s, uint32_t a, const String& n, uint32_t f)
    : section(s), index(), rva(a), flags(f), name(n)
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

Symbol& SymbolTable::insert(const Symbol& sym)
{
    for (auto& s : m_symbols) {
        if (s.name == sym.name) {
            return s;
        }
    }

    m_symbols.emplace_back(sym);
    m_symbols.back().index = (uint32_t)m_symbols.size() - 1;
    return m_symbols.back();
}

Symbol& SymbolTable::get(size_t i)
{
    return m_symbols[i];
}

} // namespace bg
