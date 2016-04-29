#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"

namespace bg {

IContext::~IContext()
{
}

bgAPI IContext* CreateContext()
{
    return new Context();
}


Context::Context()
    : m_sym(new SymbolTable(this))
    , m_str(new StringTable(this))
{
}

Context::~Context()
{
}

void Context::release()
{
    delete this;
}

size_t Context::getNumSections() const { return m_sections.size(); }
Section* Context::getSection(size_t i) { return m_sections[i].get(); }
Section* Context::createSection(const char *name, uint32 flags)
{
    auto *s = new Section(this, name, (uint32)m_sections.size(), flags);
    m_sections.emplace_back(SectionPtr(s));
    return s;
}

Context::Sections&  Context::getSections() { return m_sections; }
SymbolTable&        Context::getSymbolTable() { return *m_sym; }
StringTable&        Context::getStringTable() { return *m_str; }


bool Context::write(const char *path, Format fmt)
{
    std::fstream ofs(path, std::ios::binary | std::ios::out);
    return write(ofs, fmt);
}

bool Context::write(std::ostream &os, Format fmt)
{
    switch (fmt) {
    case Format_COFF_x86:
    {
        COFFWriter<Traits_x86> writer;
        return writer.write(*this, os);
    }
    case Format_COFF_x64:
    {
        COFFWriter<Traits_x64> writer;
        return writer.write(*this, os);
    }
    case Format_ELF_x86:
    {
        ELFWriter<Traits_x86> writer;
        return writer.write(*this, os);
    }
    case Format_ELF_x64:
    {
        ELFWriter<Traits_x64> writer;
        return writer.write(*this, os);
    }
    }
    return false;
}

} // namespace bg
