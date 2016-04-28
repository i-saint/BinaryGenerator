#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"

namespace bg {

Symbol::Symbol()
    : section(), rva(), flags(), name()
{
}

Symbol::Symbol(Section *s, uint32_t a, const String& n, uint32_t f)
    : section(s), rva(a), flags(f), name(n)
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

const Symbol& SymbolTable::insert(const Symbol& sym)
{
    for (const auto& s : m_symbols) {
        if (s.name == sym.name) {
            return s;
        }
    }

    m_symbols.emplace_back(sym);
    return m_symbols.back();
}

} // namespace bg
