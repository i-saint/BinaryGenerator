#pragma once
namespace bg {

class Section
{
public:
    Section(Context *ctx);

    Symbol addSymbol(const void *data, size_t len, const char *name);
    Relocation addRelocation(uint32_t pos, const char *name, RelocationType type);

public:
    Symbol addExternalSymbol(const char *name);
    void* ptr(uint32_t pos = 0);

private:
    Context *m_ctx;
    std::vector<char> m_data;
    uint32_t m_flags;
};

} // namespace bg
