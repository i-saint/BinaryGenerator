#pragma once
namespace bg {

enum SectionType {
    SectionType_None    = 0,
    SectionType_Read    = 1 << 0,
    SectionType_Write   = 1 << 1,
    SectionType_Execute = 1 << 2,
    SectionType_Shared  = 1 << 3,
    SectionType_Code    = 1 << 4,
    SectionType_IData   = 1 << 5,
    SectionType_Udata   = 1 << 6,

    SectionType_TextAttributes = SectionType_Read | SectionType_Execute | SectionType_Code,
    SectionType_IDataAttributes = SectionType_Read | SectionType_IData,
    SectionType_UDataAttributes = SectionType_Read | SectionType_Write | SectionType_Udata,
};

class Section
{
public:
    typedef std::vector<Relocation> Relocations;

public:
    Section(Context *ctx, const char *name, uint32_t flags);

    Symbol              addSymbol(const void *data, size_t len, const char *name);
    Relocation          addRelocation(uint32_t pos, const char *name, RelocationType type);

    const char*         getName() const;
    uint32_t            getFlags() const;
    const std::string&  getData() const;
    Relocations&        getRelocTable();

public:
    Symbol addExternalSymbol(const char *name);

private:
    Context *m_ctx;
    char m_name[8];
    uint32_t m_flags;
    std::string m_data;
    std::vector<Relocation> m_reloc;
};

} // namespace bg
