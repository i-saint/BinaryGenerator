#pragma once
namespace bg {

enum SymbolFlags {
    SymbolFlags_None        = 0,
    SymbolFlags_Static      = 1 << 0,
    SymbolFlags_External    = 1 << 1,
};

struct Symbol
{
public:
    Symbol();
    Symbol(Section *s, uint32_t a, const String& n, uint32_t f = SymbolFlags_External);

    Section     *section;
    uint32_t    rva;
    uint32_t    flags; // combination of SymbolFlags
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
