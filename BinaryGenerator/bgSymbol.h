#pragma once
namespace bg {

enum SymbolFlags {
    SymbolFlags_None = 0,
    SymbolFlags_Undefined = 1 << 0,
};

class Symbol
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
    SymbolTable(Context *ctx);
    const Symbol& insert(const Symbol& sym);

    // Body: [](const Symbol& sym) -> void
    template<class Body>
    void each(const Body& body)
    {
        for (const auto& sym : m_symbols) {
            body(sym);
        }
    }

private:
    Context *m_ctx;
    std::vector<Symbol> m_symbols;
};

} // namespace bg
