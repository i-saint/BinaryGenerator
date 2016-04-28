#pragma once
namespace bg {

// Traits: Traits_x86, Traits_x64
template<class Traits>
class COFFWriter
{
public:
    COFFWriter();
    bool write(Context& ctx, std::ostream& os);

    uint32_t translateSectionFlags(uint32_t flags);
    uint32_t translateRelocationType(RelocationType rel);

protected:
    Context *m_ctx;
    std::ostream *m_os;
};


// Traits: Traits_x86, Traits_x64
template<class Traits>
class ELFWriter
{
public:
    ELFWriter();
    bool write(Context& ctx, std::ostream& os);

protected:
    Context *m_ctx;
    std::ostream *m_os;
};

} // namespace bg
