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
class ISection;
class StringTable;

enum Format {
    Format_COFF_x86,
    Format_COFF_x64,
    Format_ELF_x86,
    Format_ELF_x64,
};

enum SectionFlag {
    SectionFlag_None    = 0,
    SectionFlag_Read    = 1 << 0,
    SectionFlag_Write   = 1 << 1,
    SectionFlag_Execute = 1 << 2,
    SectionFlag_Shared  = 1 << 3,
    SectionFlag_Code    = 1 << 4,
    SectionFlag_IData   = 1 << 5,
    SectionFlag_Udata   = 1 << 6,
    SectionFlag_Info    = 1 << 7,
    SectionFlag_Remove  = 1 << 8,

    SectionType_Text    = SectionFlag_Read | SectionFlag_Execute | SectionFlag_Code,
    SectionType_TextX   = SectionFlag_Read | SectionFlag_Write | SectionFlag_Execute | SectionFlag_Code,
    SectionType_IData   = SectionFlag_Read | SectionFlag_IData,
    SectionType_UData   = SectionFlag_Read | SectionFlag_Write | SectionFlag_Udata,
    SectionType_Info    = SectionFlag_Info | SectionFlag_Remove,
};

enum SymbolFlag {
    SymbolFlag_None     = 0,
    SymbolFlag_Static   = 1 << 0,
    SymbolFlag_External = 1 << 1,
};

enum RelocationType
{
    RelocationType_ABS,
    RelocationType_REL32,
    RelocationType_ADDR32,
    RelocationType_ADDR32NB,
    RelocationType_ADDR64,
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
    uint32      flags; // combination of SymbolFlags
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
    // flags: combination of SectionType
    virtual ISection*   createSection(const char *name, uint32 flags) = 0;

    virtual bool write(const char *path, Format fmt) = 0;
    virtual bool write(IOutputStream &os, Format fmt) = 0;
};


class ISection
{
public:
    virtual ~ISection() {}

    // add data and return position of added data
    virtual uint32 addData(const void *data, size_t len) = 0;
    // add data and symbol
    virtual Symbol addSymbol(const void *data, size_t len, const char *name, uint32 flags) = 0;
    // add symbol only
    virtual Symbol addSymbol(uint32 pos, const char *name, uint32 flags) = 0;
    // add undef symbol
    virtual Symbol addUndefinedSymbol(const char *name) = 0;

    // utilities
    Symbol addStaticSymbol(const void *data, size_t len, const char *name) { return addSymbol(data, len, name, SymbolFlag_Static); }
    Symbol addExternalSymbol(const void *data, size_t len, const char *name) { return addSymbol(data, len, name, SymbolFlag_External); }
    Symbol addStaticSymbol(uint32 pos, const char *name) { return addSymbol(pos, name, SymbolFlag_Static); }
    Symbol addExternalSymbol(uint32 pos, const char *name) { return addSymbol(pos, name, SymbolFlag_External); }

    // if symbol with name symbol_name doesn't exist, it will be added as undefined symbol
    virtual Relocation  addRelocation(uint32 pos, const char *symbol_name, RelocationType type) = 0;
    virtual Relocation  addRelocation(uint32 pos, uint32 symbol_index, RelocationType type) = 0;

    virtual const char* getName() const = 0;
    virtual uint32      getIndex() const = 0;
    virtual uint32      getFlags() const = 0;
    virtual uint32      getSize() const = 0;
    virtual char*       getData() = 0;
};


bgAPI IContext* CreateContext();

} // namespace bg
