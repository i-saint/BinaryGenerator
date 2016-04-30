#pragma once
namespace bg {

// Arch: Arch_x86, Arch_x64
template<class Arch>
class PECOFFWriter
{
public:
    PECOFFWriter();
    bool writeObj(Context& ctx, IOutputStream& os);
    bool writeExe(Context& ctx, IOutputStream& os);
    bool writeDLL(Context& ctx, IOutputStream& os);

protected:
    Context *m_ctx;
    IOutputStream *m_os;
};


// Arch: Arch_x86, Arch_x64
template<class Arch>
class ELFWriter
{
public:
    ELFWriter();
    bool writeObj(Context& ctx, IOutputStream& os);
    bool writeExe(Context& ctx, IOutputStream& os);
    bool writeDLL(Context& ctx, IOutputStream& os);

protected:
    Context *m_ctx;
    IOutputStream *m_os;
};

} // namespace bg
