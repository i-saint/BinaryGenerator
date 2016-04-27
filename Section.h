#ifndef bg_Section_h
#define bg_Section_h
namespace bg {


class Section
{
public:
    Section(Context *ctx);

    Symbol addSymbol(const void *data, size_t len, const char *name);
    Symbol addExternalSymbol(const char *name);
    Relocation addRelocation(uint32_t pos, const char *name, RelocationType type);

private:
    Context *m_ctx;
    std::vector<char> m_data;
    uint32_t m_flags;
};

} // namespace bg
#endif // bg_Section_h
