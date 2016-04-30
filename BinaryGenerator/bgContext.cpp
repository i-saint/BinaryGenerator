#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"

namespace bg {

bgAPI IContext* CreateContext()
{
    return new Context();
}

class StdOutputStream : public IOutputStream
{
public:
    StdOutputStream(std::ostream& os) : m_os(os) {}
    ~StdOutputStream() override {}
    void write(const void *data, size_t len) override {
        m_os.write((const char*)data, len);
    }
    std::ostream& m_os;
};

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
    StdOutputStream s(ofs);
    return write(s, fmt);
}

bool Context::write(IOutputStream &os, Format fmt)
{
    switch (fmt) {
    case Format_PECOFF_x86_Obj:
    {
        PECOFFWriter<Arch_x86> writer;
        return writer.writeObj(*this, os);
    }
    case Format_PECOFF_x64_Obj:
    {
        PECOFFWriter<Arch_x64> writer;
        return writer.writeObj(*this, os);
    }
    case Format_ELF_x86_Obj:
    {
        ELFWriter<Arch_x86> writer;
        return writer.writeObj(*this, os);
    }
    case Format_ELF_x64_Obj:
    {
        ELFWriter<Arch_x64> writer;
        return writer.writeObj(*this, os);
    }
    }
    return false;
}

} // namespace bg
