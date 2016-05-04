#pragma once
namespace bg {

class SymbolTable
{
public:
    using Symbols = std::vector<Symbol>;

public:
    SymbolTable(Context *ctx);
    Symbols& getSymbols();
    Symbol& addSymbol(const Symbol& sym);
    Symbol* getSymbol(size_t i);
    Symbol* findSymbol(const char *name);
    uint32 getVirtualAddress(const char *name);

private:
    Context *m_ctx;
    Symbols m_symbols;
};

} // namespace bg
