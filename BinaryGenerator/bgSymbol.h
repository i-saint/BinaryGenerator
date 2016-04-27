#pragma once
namespace bg {

enum SymbolFlags {
    SymbolFlags_None = 0,
    SymbolFlags_Undefined = 1 << 0,
};

struct Symbol
{
public:
    Symbol();
    Symbol(Section *s, uint32_t a, const String& n);

    Section     *section;
    uint32_t    rva;
    uint32_t    flags;
    String      name;
};

class SymbolTable
{
public:
    typedef std::vector<Symbol> Symbols;

public:
    SymbolTable(Context *ctx);
    Symbols& getSymbols();
    const Symbol& insert(const Symbol& sym);

private:
    Context *m_ctx;
    Symbols m_symbols;
};

} // namespace bg
