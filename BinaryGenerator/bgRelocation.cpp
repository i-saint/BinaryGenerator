#include "pch.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"

namespace bg {

Relocation::Relocation()
    : section()
    , rva()
    , sym()
    , type()
{
}

RelocationTable::RelocationTable(Context *ctx)
    : m_ctx(ctx)
{
}

} // namespace bg
