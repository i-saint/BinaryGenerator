#include "pch.h"
#include "Context.h"
#include "String.h"
#include "Symbol.h"
#include "Relocation.h"
#include "Section.h"

namespace bg {

Context::Context()
    : m_text(new Section(this))
    , m_idata(new Section(this))
    , m_udata(new Section(this))
    , m_sym(new SymbolTable(this))
    , m_reloc(new RelocationTable(this))
    , m_str(new StringTable(this))
{
}

Section&            Context::getText() { return *m_text; }
Section&            Context::getIData() { return *m_idata; }
Section&            Context::getUData() { return *m_udata; }
RelocationTable&    Context::getRelocTable() { return *m_reloc; }
SymbolTable&        Context::getSymbolTable() { return *m_sym; }
StringTable&        Context::getStringTable() { return *m_str; }


bool Context::writeCOFFx86(std::ostream &os)
{
    return false;
}

bool Context::writeCOFFx86_64(std::ostream &os)
{
    return false;
}

bool Context::writeELFx86(std::ostream &os)
{
    return false;
}

bool Context::writeELFx86_64(std::ostream &os)
{
    return false;
}

} // namespace bg
