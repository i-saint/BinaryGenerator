#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"

namespace bg {

bgAPI IPECOFFContext* CreatePECOFFContext(Architecture arch)
{
    return new PECOFFContext(arch);
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

Context::Context(Architecture arch)
    : m_arch(arch)
    , m_sym(std::make_unique<SymbolTable>(this))
    , m_str(std::make_unique<StringTable>(this))
{
}

Context::Context(const Context &from)
    : m_arch(from.m_arch)
    , m_sym(new SymbolTable(this, *from.m_sym))
    , m_str(new StringTable(this, *from.m_str))
{
    for (auto& s : from.m_sections) {
        m_sections.emplace_back(s->clone());
    }
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

const char* Context::getString(String s) const
{
    return m_str->getString(s.addr);
}

Context::Sections&      Context::getSections() { return m_sections; }
SymbolTable&            Context::getSymbolTable() { return *m_sym; }
StringTable&            Context::getStringTable() { return *m_str; }

PECOFFContext::PECOFFContext(Architecture arch)
    : super(arch)
    , m_baseaddr()
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

const char* PECOFFContext::getString(String s) const { return super::getString(s); }

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

bool PECOFFContext::writeObj(const char *path)
{
    std::fstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs) { return false; }

    m_filename = GetFileName(path);
    StdOutputStream s(ofs);
    return writeObj(s);
}
bool PECOFFContext::writeExe(const char *path)
{
    std::fstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs) { return false; }

    m_filename = GetFileName(path);
    StdOutputStream s(ofs);
    return writeExe(s);
}
bool PECOFFContext::writeDLL(const char *path)
{
    std::fstream ofs(path, std::ios::binary | std::ios::out);
    if (!ofs) { return false; }

    m_filename = GetFileName(path);
    StdOutputStream s(ofs);
    return writeDLL(s);
}

#define Impl(Func)\
    switch (m_arch) {\
    case Architecture::x86:\
        return Func<Arch_x86>(*this, os);\
        break;\
    case Architecture::x64:\
        return Func<Arch_x64>(*this, os);\
        break;\
    }\
    return false;


bool PECOFFContext::writeObj(IOutputStream &os)
{
    Impl(PECOFFWriteObj);
}

bool PECOFFContext::writeExe(IOutputStream &os)
{
    Impl(PECOFFWriteExe);
}

bool PECOFFContext::writeDLL(IOutputStream &os)
{
    Impl(PECOFFWriteDLL);
}

#undef Impl

} // namespace bg
