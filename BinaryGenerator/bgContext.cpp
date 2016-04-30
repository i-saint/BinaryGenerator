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
    , m_baseaddr()
{
}

Context::~Context()
{
}

void Context::release()
{
    delete this;
}

size_t Context::getNumSections() const
{
    return m_sections.size();
}

Section* Context::getSection(size_t i)
{
    return i < m_sections.size() ? m_sections[i].get() : nullptr;
}

Section* Context::findSection(const char *name)
{
    for (auto& s : m_sections) {
        if (strcmp(s->getName(), name) == 0) {
            return s.get();
        }
    }
    return nullptr;
}

Section* Context::createSection(const char *name, uint32 flags)
{
    if (Section* f = findSection(name)) {
        return f;
    }

    auto *s = new Section(this, name, (uint32)m_sections.size(), flags);
    m_sections.emplace_back(SectionPtr(s));
    return s;
}

void Context::setEntryPoint(const char *symbol_name)
{
    m_entrypoint = symbol_name;
}

void Context::addDLLExport(const char *symbol_name)
{
    m_dllexports.insert(symbol_name);
}

void Context::addDLLImport(const char *dll_name, const char *symbol_name)
{
    m_dllimports[dll_name].insert(symbol_name);
}

void Context::addLibrary(const char *filename)
{
    m_libraries.insert(filename);
}

Context::Sections&      Context::getSections() { return m_sections; }
SymbolTable&            Context::getSymbolTable() { return *m_sym; }
StringTable&            Context::getStringTable() { return *m_str; }

uint64                  Context::getBaseAddr() const { return m_baseaddr; }
std::string&            Context::getEntryPoint() { return m_entrypoint; }
Context::DLLExports&    Context::getDLLExports() { return m_dllexports; }
Context::DLLImports&    Context::getDLLImports() { return m_dllimports; }
Context::Libraries&     Context::getLibraries() { return m_libraries; }


bool Context::write(const char *path, Format fmt)
{
    std::fstream ofs(path, std::ios::binary | std::ios::out);
    StdOutputStream s(ofs);
    return write(s, fmt);
}

bool Context::write(IOutputStream &os, Format fmt)
{
#define Impl(Enum, Writer, Arch, Func)\
    case Enum: {\
        Writer<Arch> writer;\
        return writer.Func(*this, os);\
    }\

    switch (fmt) {
        Impl(Format_PECOFF_x86_Obj, PECOFFWriter, Arch_x86, writeObj);
        Impl(Format_PECOFF_x86_Exe, PECOFFWriter, Arch_x86, writeExe);
        Impl(Format_PECOFF_x86_DLL, PECOFFWriter, Arch_x86, writeDLL);

        Impl(Format_PECOFF_x64_Obj, PECOFFWriter, Arch_x64, writeObj);
        Impl(Format_PECOFF_x64_Exe, PECOFFWriter, Arch_x64, writeExe);
        Impl(Format_PECOFF_x64_DLL, PECOFFWriter, Arch_x64, writeDLL);

        Impl(Format_ELF_x86_Obj, ELFWriter, Arch_x86, writeObj);
        Impl(Format_ELF_x86_Exe, ELFWriter, Arch_x86, writeExe);
        Impl(Format_ELF_x86_DLL, ELFWriter, Arch_x86, writeDLL);

        Impl(Format_ELF_x64_Obj, ELFWriter, Arch_x64, writeObj);
        Impl(Format_ELF_x64_Exe, ELFWriter, Arch_x64, writeExe);
        Impl(Format_ELF_x64_DLL, ELFWriter, Arch_x64, writeDLL);
    }

#undef Impl

    return false;
}

} // namespace bg
