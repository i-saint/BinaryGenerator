#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"
#include "bgELF.h"

namespace bg {

template<class T>
ELFWriter<T>::ELFWriter()
    : m_ctx()
    , m_os()
{
}

template<class Traits> struct ELFImpl;

template<>
struct ELFImpl<Traits_x86>
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
struct ELFImpl<Traits_x64>
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

template<class T>
bool ELFWriter<T>::write(Context& ctx, std::ostream& os)
{
    typedef ELFImpl<T> Impl;

    m_ctx = &ctx;
    m_os = &os;
    auto& sections = m_ctx->getSections();
    auto& symbols = m_ctx->getSymbolTable().getSymbols();
    auto& strings = m_ctx->getStringTable().getData();

    Impl::Elf_Ehdr elf_header;
    std::vector<Impl::Elf_Shdr> elf_sections;
    std::vector<std::vector<Impl::Elf_Rela>> elf_rels;
    std::vector<Impl::Elf_Sym> elf_syms;

    elf_sections.resize(sections.size());
    elf_rels.resize(sections.size());
    elf_syms.resize(symbols.size());


    return false;
}

template class ELFWriter<Traits_x86>;
template class ELFWriter<Traits_x64>;

} // namespace bg
