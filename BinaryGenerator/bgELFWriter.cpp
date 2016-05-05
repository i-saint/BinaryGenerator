#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"
#include "bgELF.h"

namespace bg {

template<class Traits> struct ELFImpl;

template<>
struct ELFImpl<Arch_x86>
{
    static const int elf_class = ELFCLASS32;

    typedef Elf32_Ehdr Elf_Ehdr;
    typedef Elf32_Phdr Elf_Phdr;
    typedef Elf32_Shdr Elf_Shdr;
    typedef Elf32_Sym  Elf_Sym;
    typedef Elf32_Rel  Elf_Rel;
    typedef Elf32_Rela Elf_Rela;
    typedef Elf32_Dyn  Elf_Dyn;
};

template<>
struct ELFImpl<Arch_x64>
{
    static const int elf_class = ELFCLASS64;

    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym  Elf_Sym;
    typedef Elf64_Rel  Elf_Rel;
    typedef Elf64_Rela Elf_Rela;
    typedef Elf64_Dyn  Elf_Dyn;
};

// Arch: Arch_x86, Arch_x64
template<class Arch>
class ELFWriter : public ELFImpl<Arch>
{
public:
    ELFWriter(Context& ctx, IOutputStream& os);
    bool writeObj();
    bool writeExe();
    bool writeDLL();

private:
    Context& m_ctx;
    IOutputStream& m_os;
};

template<class T>
ELFWriter<T>::ELFWriter(Context& ctx, IOutputStream& os)
    : m_ctx(ctx)
    , m_os(os)
{
}


template<class T>
bool ELFWriter<T>::writeObj()
{
    /*
    auto& sections = m_ctx.getSections();
    auto& symbols = m_ctx.getSymbolTable().getSymbols();
    auto& strings = m_ctx.getStringTable().getData();

    Elf_Ehdr elf_header;
    std::vector<Elf_Shdr> elf_sections;
    std::vector<std::vector<Elf_Rela>> elf_rels;
    std::vector<Elf_Sym> elf_syms;

    elf_sections.resize(sections.size());
    elf_rels.resize(sections.size());
    elf_syms.resize(symbols.size());
    */

    return false;
}

template<class T>
bool ELFWriter<T>::writeExe()
{
    return false;
}

template<class T>
bool ELFWriter<T>::writeDLL()
{
    return false;
}


template<class Arch> bool ELFWriteObj(Context& ctx, IOutputStream& os)
{
    ELFWriter<Arch> writer(ctx, os);
    return writer.writeObj();
}
template<class Arch> bool ELFWriteExe(Context& ctx, IOutputStream& os)
{
    ELFWriter<Arch> writer(ctx, os);
    return writer.writeExe();
}
template<class Arch> bool ELFWriteDLL(Context& ctx, IOutputStream& os)
{
    ELFWriter<Arch> writer(ctx, os);
    return writer.writeDLL();
}


template bool ELFWriteObj<Arch_x86>(Context& ctx, IOutputStream& os);
template bool ELFWriteExe<Arch_x86>(Context& ctx, IOutputStream& os);
template bool ELFWriteDLL<Arch_x86>(Context& ctx, IOutputStream& os);
template bool ELFWriteObj<Arch_x64>(Context& ctx, IOutputStream& os);
template bool ELFWriteExe<Arch_x64>(Context& ctx, IOutputStream& os);
template bool ELFWriteDLL<Arch_x64>(Context& ctx, IOutputStream& os);

} // namespace bg
