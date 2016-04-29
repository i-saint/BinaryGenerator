#pragma once
namespace bg {

enum Format {
    Format_COFF_x86,
    Format_COFF_x64,
    Format_ELF_x86,
    Format_ELF_x64,

    //Format_COFF_ARM32,
    //Format_COFF_ARM64,
    //Format_ELF_ARM32,
    //Format_ELF_ARM64,
};

class Context
{
public:
    typedef std::unique_ptr<Section>            SectionPtr;
    typedef std::unique_ptr<SymbolTable>        SymbolTablePtr;
    typedef std::unique_ptr<StringTable>        StringTablePtr;
    typedef std::vector<SectionPtr>             Sections;

public:
    Context();

    size_t              getNumSections() const;
    Section*            getSection(size_t i);
    // flags: combination of SectionType
    Section*            createSection(const char *name, uint32_t flags);

    Sections&           getSections();
    SymbolTable&        getSymbolTable();
    StringTable&        getStringTable();

    bool write(const char *path, Format fmt);
    bool write(std::ostream &os, Format fmt);

private:
    Sections            m_sections;
    SymbolTablePtr      m_sym;
    StringTablePtr      m_str;
};

} // namespace bg
