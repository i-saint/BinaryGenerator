#pragma once
namespace bg {

class Context : public IContext
{
public:
    Context();
    ~Context() override;
    void        release() override;

    size_t      getNumSections() const override;
    Section*    getSection(size_t i) override;
    Section*    findSection(const char *name) override;
    // flags: combination of SectionType
    Section*    createSection(const char *name, uint32 flags) override;

    void        setBaseAddress(uint64 addr) override;
    void        setSubsystem(Subsystem ss) override;
    void        setEntryPoint(const char *symbol_name) override;
    void        addDLLExport(const char *symbol_name) override;
    void        addDLLImport(const char *dll_name, const char *symbol_name) override;
    void        addLibrary(const char *filename) override;

    bool        write(const char *path, Format fmt) override;
    bool        write(IOutputStream &os, Format fmt) override;

public:
    typedef std::unique_ptr<Section>        SectionPtr;
    typedef std::unique_ptr<SymbolTable>    SymbolTablePtr;
    typedef std::unique_ptr<StringTable>    StringTablePtr;
    typedef std::vector<SectionPtr>         Sections;
    typedef std::set<std::string>           DLLExports;
    typedef std::map<
        std::string, std::set<std::string>> DLLImports;
    typedef std::set<std::string>           Libraries;

    Sections&       getSections();
    SymbolTable&    getSymbolTable();
    StringTable&    getStringTable();

    Subsystem       getSubsystem() const;
    uint64          getBaseAddress() const;
    std::string&    getEntryPoint();
    DLLExports&     getDLLExports();
    DLLImports&     getDLLImports();
    Libraries&      getLibraries();

private:
    Sections            m_sections;
    SymbolTablePtr      m_sym;
    StringTablePtr      m_str;

    uint64              m_baseaddr;
    Subsystem           m_subsystem;
    std::string         m_entrypoint;
    DLLExports          m_dllexports;
    DLLImports          m_dllimports;
    Libraries           m_libraries;
};

} // namespace bg
