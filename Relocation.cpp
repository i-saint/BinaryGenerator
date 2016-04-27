#include "pch.h"
#include "Context.h"
#include "String.h"
#include "Symbol.h"
#include "Relocation.h"

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
