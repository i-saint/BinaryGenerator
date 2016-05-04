#pragma once
namespace bg {

class Section : public ISection
{
public:
    Section(Context *ctx, const char *name, uint32 index, SectionFlag flags);
    ~Section() override;

    // return position of added data
    uint32 addData(const void *data, size_t len) override;
    // add data and symbol
    Symbol addSymbol(const void *data, size_t len, const char *name, SymbolFlag flags) override;
    // add symbol only
    Symbol addSymbol(uint32 pos, const char *name, SymbolFlag flags) override;
    // add undef symbol
    Symbol addUndefinedSymbol(const char *name) override;

    Relocation  addRelocation(uint32 pos, const char *symbol_name, RelocationType type) override;
    Relocation  addRelocation(uint32 pos, uint32 symbol_index, RelocationType type) override;

    const char* getName() const override;
    uint32      getIndex() const override;
    SectionFlag getFlags() const override;
    uint32      getVirtualAddress() const override;
    uint32      getSize() const override;
    char*       getData() override;

    void        setVirtualAddress(uint32 va) override;

public:
    using Relocations = std::vector<Relocation>;
    Relocations&    getRelocations();

private:
    Context *m_ctx;
    char m_name[8];
    uint32 m_index;
    SectionFlag m_flags;
    uint32 m_virtual_addr;
    std::string m_data;
    Relocations m_reloc;
};

} // namespace bg
