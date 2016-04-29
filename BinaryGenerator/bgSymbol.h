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
    uint32_t    addr;
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
    Symbol& addSymbol(const Symbol& sym);
    Symbol* getSymbol(size_t i);
    Symbol* findSymbol(const char *name);

private:
    Context *m_ctx;
    Symbols m_symbols;
};

} // namespace bg
