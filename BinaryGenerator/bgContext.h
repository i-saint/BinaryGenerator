#pragma once
namespace bg {

class Context : public IContext
{
public:
    Context();
    ~Context() override;
    void            release() override;

    size_t          getNumSections() const override;
    Section*        getSection(size_t i) override;
    // flags: combination of SectionType
    Section*        createSection(const char *name, uint32 flags) override;

    bool            write(const char *path, Format fmt) override;
    bool            write(IOutputStream &os, Format fmt) override;

public:
    typedef std::unique_ptr<Section>            SectionPtr;
    typedef std::unique_ptr<SymbolTable>        SymbolTablePtr;
    typedef std::unique_ptr<StringTable>        StringTablePtr;
    typedef std::vector<SectionPtr>             Sections;

    Sections&       getSections();
    SymbolTable&    getSymbolTable();
    StringTable&    getStringTable();

private:
    Sections            m_sections;
    SymbolTablePtr      m_sym;
    StringTablePtr      m_str;
};

} // namespace bg
