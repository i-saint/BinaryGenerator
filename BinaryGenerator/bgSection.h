#pragma once
namespace bg {

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


class Section
{
public:
    typedef std::vector<Relocation> Relocations;

public:
    Section(Context *ctx, const char *name, uint32_t index, uint32_t flags);

    Symbol              addSymbol(const void *data, size_t len, const char *name, uint32_t flags);
    Symbol              addStaticSymbol(const void *data, size_t len, const char *name);
    Symbol              addExternalSymbol(const void *data, size_t len, const char *name);

    Relocation          addRelocation(uint32_t pos, const char *symbol_name, RelocationType type);
    Relocation          addRelocation(uint32_t pos, uint32_t symbol_index, RelocationType type);

    const char*         getName() const;
    uint32_t            getIndex() const;
    uint32_t            getFlags() const;
    const std::string&  getData() const;
    Relocations&        getRelocations();

public:
    Symbol              addExternalSymbol(const char *name);

private:
    Context *m_ctx;
    char m_name[8];
    uint32_t m_index;
    uint32_t m_flags;
    std::string m_data;
    std::vector<Relocation> m_reloc;
};

} // namespace bg
