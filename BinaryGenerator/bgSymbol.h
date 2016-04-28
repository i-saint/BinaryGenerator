#pragma once
namespace bg {

enum SymbolFlag {
    SymbolFlag_None        = 0,
    SymbolFlag_Static      = 1 << 0,
    SymbolFlag_External    = 1 << 1,
};

struct Symbol
{
public:
    Symbol();
    Symbol(Section *s, uint32_t a, const String& n, uint32_t f = SymbolFlag_External);

    Section     *section;
    uint32_t    index;
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
    Symbol& insert(const Symbol& sym);
    Symbol& get(size_t i);

private:
    Context *m_ctx;
    Symbols m_symbols;
};

} // namespace bg
