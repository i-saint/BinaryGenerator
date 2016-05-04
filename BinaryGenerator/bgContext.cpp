#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"

namespace bg {

bgAPI IPECOFFContext* CreatePECOFFContext()
{
    return new PECOFFContext();
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

Section* Context::createSection(const char *name, SectionFlag flags)
{
    if (Section* f = findSection(name)) {
        return f;
    }

    auto *s = new Section(this, name, (uint32)m_sections.size(), flags);
    m_sections.emplace_back(SectionPtr(s));
    return s;
}

Context::Sections&      Context::getSections() { return m_sections; }
SymbolTable&            Context::getSymbolTable() { return *m_sym; }
StringTable&            Context::getStringTable() { return *m_str; }


PECOFFContext::PECOFFContext()
    : m_baseaddr()
    , m_subsystem(Subsystem::GUI)
{
}
PECOFFContext::~PECOFFContext()
{
}

void PECOFFContext::release() { delete this; }
size_t PECOFFContext::getNumSections() const { return super::getNumSections(); }
Section* PECOFFContext::getSection(size_t i) { return super::getSection(i); }
Section* PECOFFContext::findSection(const char *name) { return super::findSection(name); }
Section* PECOFFContext::createSection(const char *name, SectionFlag flags) { return super::createSection(name, flags); }

void PECOFFContext::setBaseAddress(uint64 addr)
{
    m_baseaddr = addr;
}

void PECOFFContext::setSubsystem(Subsystem ss)
{
    m_subsystem = ss;
}

void PECOFFContext::setEntryPoint(const char *symbol_name)
{
    m_entrypoint = symbol_name;
}

void PECOFFContext::addDLLExport(const char *symbol_name)
{
    m_dllexports.insert(symbol_name);
}

void PECOFFContext::addDLLImport(const char *dll_name, const char *symbol_name)
{
    m_dllimports[dll_name].insert(symbol_name);
}

void PECOFFContext::addLibrary(const char *filename)
{
    m_libraries.insert(filename);
}

std::string&            PECOFFContext::getFileName() { return m_filename; }
Subsystem               PECOFFContext::getSubsystem() const { return m_subsystem; }
uint64                  PECOFFContext::getBaseAddress() const { return m_baseaddr; }
std::string&            PECOFFContext::getEntryPoint() { return m_entrypoint; }
PECOFFContext::DLLExports&    PECOFFContext::getDLLExports() { return m_dllexports; }
PECOFFContext::DLLImports&    PECOFFContext::getDLLImports() { return m_dllimports; }
PECOFFContext::Libraries&     PECOFFContext::getLibraries() { return m_libraries; }

bool PECOFFContext::write(const char *path, Format fmt)
{
    {
        size_t len = strlen(path);
        size_t separator = 0;
        for (size_t i = 0; i < len; ++i) {
            if (path[i] == '/' || path[i] == '\\') { separator = i + 1; }
        }
        m_filename = path + separator;
    }

    std::fstream ofs(path, std::ios::binary | std::ios::out);
    StdOutputStream s(ofs);
    return write(s, fmt);
}

bool PECOFFContext::write(IOutputStream &os, Format fmt)
{
#define Impl(Enum, Func, Arch) case Enum: { return Func<Arch>(*this, os); }

    switch (fmt) {
        Impl(Format::PECOFF_x86_Obj, PECOFFWriteObj, Arch_x86);
        Impl(Format::PECOFF_x86_Exe, PECOFFWriteExe, Arch_x86);
        Impl(Format::PECOFF_x86_DLL, PECOFFWriteDLL, Arch_x86);

        Impl(Format::PECOFF_x64_Obj, PECOFFWriteObj, Arch_x64);
        Impl(Format::PECOFF_x64_Exe, PECOFFWriteExe, Arch_x64);
        Impl(Format::PECOFF_x64_DLL, PECOFFWriteDLL, Arch_x64);

        Impl(Format::ELF_x86_Obj, ELFWriteObj, Arch_x86);
        Impl(Format::ELF_x86_Exe, ELFWriteExe, Arch_x86);
        Impl(Format::ELF_x86_DLL, ELFWriteDLL, Arch_x86);

        Impl(Format::ELF_x64_Obj, ELFWriteObj, Arch_x64);
        Impl(Format::ELF_x64_Exe, ELFWriteExe, Arch_x64);
        Impl(Format::ELF_x64_DLL, ELFWriteDLL, Arch_x64);
    }

#undef Impl

    return false;
}

} // namespace bg
