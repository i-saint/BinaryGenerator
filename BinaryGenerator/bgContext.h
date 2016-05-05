#pragma once
namespace bg {

class Context
{
public:
    Context(Architecture arch);
    Context(const Context &from);
    virtual ~Context();

    size_t      getNumSections() const;
    Section*    getSection(size_t i);
    Section*    findSection(const char *name);
    // flags: combination of SectionType
    Section*    createSection(const char *name, SectionFlag flags);
    
    const char* getString(String s) const;

public:
    using SectionPtr     = std::unique_ptr<Section>;
    using SymbolTablePtr = std::unique_ptr<SymbolTable>;
    using StringTablePtr = std::unique_ptr<StringTable>;
    using Sections       = std::vector<SectionPtr>;

    Sections&       getSections();
    SymbolTable&    getSymbolTable();
    StringTable&    getStringTable();

protected:
    Architecture        m_arch;
    Sections            m_sections;
    SymbolTablePtr      m_sym;
    StringTablePtr      m_str;
};


class PECOFFContext : public Context, public IPECOFFContext
{
typedef Context super;
public:
    PECOFFContext(Architecture arch);
    PECOFFContext(const PECOFFContext &base) = default;
    ~PECOFFContext() override;
    void        release() override;

    size_t      getNumSections() const override;
    Section*    getSection(size_t i) override;
    Section*    findSection(const char *name) override;
    Section*    createSection(const char *name, SectionFlag flags) override;

    const char* getString(String s) const override;

    void        setBaseAddress(uint64 addr) override;
    void        setSubsystem(Subsystem ss) override;
    void        setEntryPoint(const char *symbol_name) override;
    void        addDLLExport(const char *symbol_name) override;
    void        addDLLImport(const char *dll_name, const char *symbol_name) override;
    void        addLibrary(const char *filename) override;

    bool        writeObj(const char *path) override;
    bool        writeExe(const char *path) override;
    bool        writeDLL(const char *path) override;
    bool        writeObj(IOutputStream &os) override;
    bool        writeExe(IOutputStream &os) override;
    bool        writeDLL(IOutputStream &os) override;

public:
    using DLLExports = std::set<std::string>;
    using DLLImports = std::map<std::string, std::set<std::string>>;
    using Libraries  = std::set<std::string>;

    std::string&    getFileName();
    Subsystem       getSubsystem() const;
    uint64          getBaseAddress() const;
    std::string&    getEntryPoint();
    DLLExports&     getDLLExports();
    DLLImports&     getDLLImports();
    Libraries&      getLibraries();

private:
    std::string m_filename;
    uint64      m_baseaddr;
    Subsystem   m_subsystem;
    std::string m_entrypoint;
    DLLExports  m_dllexports;
    DLLImports  m_dllimports;
    Libraries   m_libraries;
};

} // namespace bg
