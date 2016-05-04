#pragma once

#define bgCLinkage extern "C"
#ifdef _WIN32
    #ifndef bgStaticLink
        #ifdef bgImpl
            #define bgAPI __declspec(dllexport)
        #else
            #define bgAPI __declspec(dllimport)
            #pragma comment(lib, "BinaryGenerator.lib")
        #endif
    #else
        #define bgAPI
        #ifdef bgImpl
        #else
            #pragma comment(lib, "BinaryGenerator_s.lib")
        #endif
    #endif
#else
    #define bgAPI
#endif

namespace bg {
    
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

class IOutputStream;
class IContext;
class IPECOFFContext;
class IELFContext;
class ISection;
class StringTable;

enum class Format
{
    PECOFF_x86_Obj = 0x100,
    PECOFF_x86_Exe,
    PECOFF_x86_DLL,

    PECOFF_x64_Obj = 0x200,
    PECOFF_x64_Exe,
    PECOFF_x64_DLL,

    ELF_x86_Obj = 0x300,
    ELF_x86_Exe,
    ELF_x86_DLL,

    ELF_x64_Obj = 0x400,
    ELF_x64_Exe,
    ELF_x64_DLL,
};

enum class Subsystem
{
    CUI,
    GUI,
};

enum class SectionFlag
{
    None    = 0,
    Read    = 1 << 0,
    Write   = 1 << 1,
    Execute = 1 << 2,
    Shared  = 1 << 3,
    Code    = 1 << 4,
    IData   = 1 << 5,
    UData   = 1 << 6,
    Info    = 1 << 7,
    Remove  = 1 << 8,

    TextSection = Read | Execute | Code,
    TextXSection = Read | Write | Execute | Code,
    IDataSection = Read | IData,
    UDataSection = Read | Write | UData,
    InfoSection = Info | Remove,
};
inline SectionFlag operator|(SectionFlag a, SectionFlag b) { return (SectionFlag)(int(a) | int(b)); }
inline int operator&(SectionFlag a, SectionFlag b) { return (int(a) & int(b)); }

enum class SymbolFlag
{
    None = 0,
    Static = 1 << 0,
    External = 1 << 1,
};
inline SymbolFlag operator|(SymbolFlag a, SymbolFlag b) { return (SymbolFlag)(int(a) | int(b)); }
inline int operator&(SymbolFlag a, SymbolFlag b) { return (int(a) & int(b)); }

enum class RelocationType
{
    ABS,
    REL32,
    ADDR32,
    ADDR32NB,
    ADDR64,
};

struct String
{
public:
    const char* str() const;

    StringTable *table;
    uint32      addr;
};

struct Symbol
{
    ISection    *section;
    uint32      index;
    uint32      addr;
    SymbolFlag  flags; // combination of SymbolFlags
    String      name;
};

struct Relocation
{
    ISection        *section;
    uint32          addr;
    uint32          symbol_index;
    RelocationType  type;
};


// make derived class and pass it to IContext::write() if you want to customize output
class IOutputStream
{
public:
    virtual ~IOutputStream() {}
    virtual void write(const void *data, size_t len) = 0;
};


class IContext
{
public:
    virtual ~IContext() {}
    virtual void        release() = 0;

    virtual size_t      getNumSections() const = 0;
    virtual ISection*   getSection(size_t i) = 0;
    virtual ISection*   findSection(const char *name) = 0;
    // flags: combination of SectionType
    virtual ISection*   createSection(const char *name, SectionFlag flags) = 0;
};

class IPECOFFContext : public IContext
{
public:
    // only relevant for executable
    virtual void        setBaseAddress(uint64 addr) = 0;
    // subsystem for windows. only relevant for executable
    virtual void        setSubsystem(Subsystem ss) = 0;
    // only relevant for executable
    virtual void        setEntryPoint(const char *symbol_name) = 0;
    virtual void        addDLLExport(const char *symbol_name) = 0;
    // only relevant for executable
    virtual void        addDLLImport(const char *dll_name, const char *symbol_name) = 0;
    // add library dependency. only relevant for non-executable
    virtual void        addLibrary(const char *filename) = 0;

    virtual bool        write(const char *path, Format fmt) = 0;
    virtual bool        write(IOutputStream &os, Format fmt) = 0;
};


class ISection
{
public:
    virtual ~ISection() {}

    // add data and return position of added data. data can be null
    virtual uint32 addData(const void *data, size_t len) = 0;
    // add data and symbol. data can be null
    virtual Symbol addSymbol(const void *data, size_t len, const char *name, SymbolFlag flags) = 0;
    // add symbol only
    virtual Symbol addSymbol(uint32 pos, const char *name, SymbolFlag flags) = 0;
    // add undef symbol
    virtual Symbol addUndefinedSymbol(const char *name) = 0;

    // utilities
    Symbol addStaticSymbol(const void *data, size_t len, const char *name) { return addSymbol(data, len, name, SymbolFlag::Static); }
    Symbol addExternalSymbol(const void *data, size_t len, const char *name) { return addSymbol(data, len, name, SymbolFlag::External); }
    Symbol addStaticSymbol(uint32 pos, const char *name) { return addSymbol(pos, name, SymbolFlag::Static); }
    Symbol addExternalSymbol(uint32 pos, const char *name) { return addSymbol(pos, name, SymbolFlag::External); }

    // if symbol with name symbol_name doesn't exist, it will be added as undefined symbol
    virtual Relocation  addRelocation(uint32 pos, const char *symbol_name, RelocationType type) = 0;
    virtual Relocation  addRelocation(uint32 pos, uint32 symbol_index, RelocationType type) = 0;

    virtual const char* getName() const = 0;
    virtual uint32      getIndex() const = 0;
    virtual SectionFlag getFlags() const = 0;
    virtual uint32      getVirtualAddress() const = 0;
    virtual uint32      getSize() const = 0;
    virtual char*       getData() = 0;

    virtual void        setVirtualAddress(uint32 va)  = 0;
};


bgAPI IPECOFFContext* CreatePECOFFContext();
bgAPI IELFContext* CreateELFContext();

} // namespace bg
