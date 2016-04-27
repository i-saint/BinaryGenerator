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
    Section(Context *ctx, const char *name, uint32_t flags);

    Symbol addSymbol(const void *data, size_t len, const char *name);
    Relocation addRelocation(uint32_t pos, const char *name, RelocationType type);

public:
    Symbol addExternalSymbol(const char *name);
    void* ptr(uint32_t pos = 0);

private:
    Context *m_ctx;
    char m_name[8];
    uint32_t m_flags;
    std::vector<char> m_data;
};

} // namespace bg
