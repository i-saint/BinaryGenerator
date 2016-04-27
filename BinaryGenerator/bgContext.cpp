#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"
#include "bgSection.h"
#include "bgCOFF.h"

namespace bg {

Context::Context()
    : m_sym(new SymbolTable(this))
    , m_reloc(new RelocationTable(this))
    , m_str(new StringTable(this))
{
}

size_t Context::getNumSections() const { return m_sections.size(); }
Section* Context::getSection(size_t i) { return m_sections[i].get(); }
Section* Context::createSection(const char *name, uint32_t flags)
{
    auto *s = new Section(this, name, flags);
    m_sections.emplace_back(SectionPtr(s));
    return s;
}

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
