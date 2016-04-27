#include "pch.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"

namespace bg {

Symbol::Symbol()
    : section(), rva(), flags(), name()
{
}

Symbol::Symbol(Section *s, uint32_t a, const String& n)
    : section(s), rva(a), flags(), name(n)
{
}

SymbolTable::SymbolTable(Context *ctx)
    : m_ctx(ctx)
{
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
