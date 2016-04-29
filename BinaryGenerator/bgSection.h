#pragma once
namespace bg {

class Section : public ISection
{
public:
    Section(Context *ctx, const char *name, uint32 index, uint32 flags);
    ~Section() override;

    // return position of added data
    uint32 addData(const void *data, uint32 len) override;
    // add data and symbol
    Symbol addSymbol(const void *data, uint32 len, const char *name, uint32 flags) override;
    // add symbol only
    Symbol addSymbol(uint32 pos, const char *name, uint32 flags) override;
    // add undef symbol
    Symbol addUndefinedSymbol(const char *name) override;

    Relocation  addRelocation(uint32 pos, const char *symbol_name, RelocationType type) override;
    Relocation  addRelocation(uint32 pos, uint32 symbol_index, RelocationType type) override;

    const char* getName() const override;
    uint32      getIndex() const override;
    uint32      getFlags() const override;
    uint32      getSize() const override;
    char*       getData() override;

public:
    typedef std::vector<Relocation> Relocations;
    Relocations&    getRelocations();

private:
    Context *m_ctx;
    char m_name[8];
    uint32 m_index;
    uint32 m_flags;
    std::string m_data;
    std::vector<Relocation> m_reloc;
};

} // namespace bg
