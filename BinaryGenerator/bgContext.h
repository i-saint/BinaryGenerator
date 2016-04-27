#pragma once
namespace bg {

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

    bool writeCOFFx86(std::ostream &os);
    bool writeCOFFx86_64(std::ostream &os);
    bool writeELFx86(std::ostream &os);
    bool writeELFx86_64(std::ostream &os);

private:
    Sections            m_sections;
    SymbolTablePtr      m_sym;
    StringTablePtr      m_str;
};

} // namespace bg
