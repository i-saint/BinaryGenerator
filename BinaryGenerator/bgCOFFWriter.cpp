#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"
#include "bgCOFF.h"


// structure of COFF file

//--------------------------//
// IMAGE_FILE_HEADER        //
//--------------------------//
// IMAGE_SECTION_HEADER     //
//  * num sections          //
//--------------------------//
//                          //
//                          //
//                          //
// section data             //
//  * num sections          //
//                          //
//                          //
//--------------------------//
// IMAGE_SYMBOL             //
//  * num symbols           //
//--------------------------//
// string table             //
//--------------------------//

namespace bg {

struct PECOFFTypesCommon
{
    typedef IMAGE_DOS_HEADER        IMAGE_DOS_HEADER;
    typedef IMAGE_FILE_HEADER       IMAGE_FILE_HEADER;
    typedef IMAGE_SECTION_HEADER    IMAGE_SECTION_HEADER;
    typedef IMAGE_SYMBOL            IMAGE_SYMBOL;
    typedef IMAGE_RELOCATION        IMAGE_RELOCATION;
    typedef IMAGE_EXPORT_DIRECTORY  IMAGE_EXPORT_DIRECTORY;
};

template<class Arch> struct PECOFFTypes;

template<>
struct PECOFFTypes<Arch_x86> : PECOFFTypesCommon
{
    typedef IMAGE_NT_HEADERS32          IMAGE_NT_HEADERS;
    typedef IMAGE_OPTIONAL_HEADER32     IMAGE_OPTIONAL_HEADER;
};

template<>
struct PECOFFTypes<Arch_x64> : PECOFFTypesCommon
{
    typedef IMAGE_NT_HEADERS64          IMAGE_NT_HEADERS;
    typedef IMAGE_OPTIONAL_HEADER64     IMAGE_OPTIONAL_HEADER;
};



template<class Arch>
struct PECOFFImpl : public PECOFFTypes<Arch>
{
    static WORD getMachineType();
    static uint32 translateSectionFlags(uint32 flags);
    static uint32 translateRelocationType(RelocationType rel);
};

template<> WORD PECOFFImpl<Arch_x86>::getMachineType() { return IMAGE_FILE_MACHINE_I386; }
template<> WORD PECOFFImpl<Arch_x64>::getMachineType() { return IMAGE_FILE_MACHINE_AMD64; }


template<class Arch>
uint32 PECOFFImpl<Arch>::translateSectionFlags(uint32 flags)
{
    uint32 r = 0;
    if ((flags & SectionFlag_Code)) {
        r |= IMAGE_SCN_CNT_CODE;
        r |= IMAGE_SCN_ALIGN_16BYTES;
    }
    if ((flags & SectionFlag_IData)) {
        r |= IMAGE_SCN_CNT_INITIALIZED_DATA;
        r |= IMAGE_SCN_ALIGN_16BYTES;
    }
    if ((flags & SectionFlag_UData)) {
        r |= IMAGE_SCN_CNT_UNINITIALIZED_DATA;
        r |= IMAGE_SCN_ALIGN_16BYTES;
    }
    if ((flags & SectionFlag_Info)) {
        r |= IMAGE_SCN_LNK_INFO;
        r |= IMAGE_SCN_ALIGN_1BYTES;
    }
    if ((flags & SectionFlag_Read)) {
        r |= IMAGE_SCN_MEM_READ;
    }
    if ((flags & SectionFlag_Write)) {
        r |= IMAGE_SCN_MEM_WRITE;
    }
    if ((flags & SectionFlag_Execute)) {
        r |= IMAGE_SCN_MEM_EXECUTE;
    }
    if ((flags & SectionFlag_Shared)) {
        r |= IMAGE_SCN_MEM_SHARED;
    }
    if ((flags & SectionFlag_Remove)) {
        r |= IMAGE_SCN_LNK_REMOVE;
    }
    return r;
}

template<>
uint32 PECOFFImpl<Arch_x86>::translateRelocationType(RelocationType rel)
{
    switch (rel) {
    case RelocationType_ABS:        return IMAGE_REL_I386_ABSOLUTE;
    case RelocationType_REL32:      return IMAGE_REL_I386_REL32;
    case RelocationType_ADDR32:     return IMAGE_REL_I386_DIR32;
    case RelocationType_ADDR32NB:   return IMAGE_REL_I386_DIR32NB;
    case RelocationType_ADDR64:     break;
    }
    return 0;
}

template<>
uint32 PECOFFImpl<Arch_x64>::translateRelocationType(RelocationType rel)
{
    switch (rel) {
    case RelocationType_ABS:        return IMAGE_REL_AMD64_ABSOLUTE;
    case RelocationType_REL32:      return IMAGE_REL_AMD64_REL32;
    case RelocationType_ADDR32:     return IMAGE_REL_AMD64_ADDR32;
    case RelocationType_ADDR32NB:   return IMAGE_REL_AMD64_ADDR32NB;
    case RelocationType_ADDR64:     return IMAGE_REL_AMD64_ADDR64;
    }
    return 0;
}


template<class Arch>
PECOFFWriter<Arch>::PECOFFWriter()
    : m_ctx()
    , m_os()
{
}

template<class Arch>
bool PECOFFWriter<Arch>::writeObj(Context& ctx, IOutputStream& os)
{
    typedef PECOFFImpl<Arch> Impl;

    m_ctx = &ctx;
    m_os = &os;
    auto& sections = m_ctx->getSections();
    auto& symbols = m_ctx->getSymbolTable().getSymbols();
    auto& strings = m_ctx->getStringTable().getData();

    Impl::IMAGE_FILE_HEADER coff_header;
    std::vector<Impl::IMAGE_SECTION_HEADER> coff_sects;
    std::vector<std::vector<Impl::IMAGE_RELOCATION>> coff_rels;
    std::vector<Impl::IMAGE_SYMBOL> coff_syms;

    DWORD pos_sections = DWORD(
        IMAGE_SIZEOF_FILE_HEADER +
        IMAGE_SIZEOF_SECTION_HEADER * sections.size()
    );
    DWORD pos_symbols = pos_sections;
    for (auto& s : sections) {
        pos_symbols += s->getSize();
        pos_symbols += (DWORD)(IMAGE_SIZEOF_RELOCATION * s->getRelocations().size());
    }

    coff_header.Machine = Impl::getMachineType();
    coff_header.NumberOfSections = (WORD)sections.size();
    coff_header.TimeDateStamp = 0;
    coff_header.PointerToSymbolTable = pos_symbols;
    coff_header.NumberOfSymbols = (DWORD)symbols.size();
    coff_header.SizeOfOptionalHeader = 0;
    coff_header.Characteristics = 0;

    coff_sects.resize(sections.size());
    coff_rels.resize(sections.size());
    coff_syms.resize(symbols.size());

    // build symbol info
    for (size_t si = 0; si < symbols.size(); ++si) {
        auto& sym = symbols[si];
        auto& isym = coff_syms[si];

        isym.N.Name.Short = 0;
        isym.N.Name.Long = sym.name.addr;
        isym.Value = sym.addr;
        isym.SectionNumber = sym.section ? sym.section->getIndex() + 1 : IMAGE_SYM_UNDEFINED;
        isym.Type = IMAGE_SYM_TYPE_NULL;
        isym.StorageClass = IMAGE_SYM_CLASS_NULL;
        if ((sym.flags & SymbolFlag_Static)) {
            isym.StorageClass = IMAGE_SYM_CLASS_STATIC;
        }
        if ((sym.flags & SymbolFlag_External)) {
            isym.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
        }
        isym.NumberOfAuxSymbols = 0;
    }

    // build section info
    for (size_t si = 0; si < sections.size(); ++si) {
        auto& section = sections[si];
        DWORD datasize = section->getSize();
        auto& rels = section->getRelocations();

        auto& sh = coff_sects[si];
        auto& irels = coff_rels[si];

        memcpy(sh.Name, section->getName(), 8);
        sh.VirtualAddress = 0;
        sh.SizeOfRawData = datasize;
        sh.PointerToRawData = pos_sections;
        sh.PointerToRelocations = rels.empty() ? 0 : pos_sections + datasize;
        sh.PointerToLinenumbers = 0;
        sh.NumberOfRelocations = (WORD)rels.size();
        sh.NumberOfLinenumbers = 0;
        sh.Characteristics = Impl::translateSectionFlags(section->getFlags());

        // build relocation info
        irels.resize(rels.size());
        for (size_t ri = 0; ri < rels.size(); ++ri) {
            auto& rel = rels[ri];
            auto& coff = irels[ri];
            coff.DUMMYUNIONNAME.VirtualAddress = rel.addr;
            coff.SymbolTableIndex = rel.symbol_index;
            coff.Type = Impl::translateRelocationType(rel.type);
        }

        pos_sections += sh.SizeOfRawData;
        pos_sections += IMAGE_SIZEOF_RELOCATION * sh.NumberOfRelocations;
    }


    // write actual data

    // file header
    m_os->write(&coff_header, IMAGE_SIZEOF_FILE_HEADER);

    // section headers
    for (auto& sh : coff_sects) {
        m_os->write(&sh, IMAGE_SIZEOF_SECTION_HEADER);
    }

    // section contents
    for (size_t si = 0; si < sections.size(); ++si) {
        auto& section = sections[si];
        if (section->getData()) {
            m_os->write(section->getData(), section->getSize());
        }
        if (!coff_rels.empty()) {
            for (auto& r : coff_rels[si]) {
                m_os->write(&r, IMAGE_SIZEOF_RELOCATION);
            }
        }
    }

    // symbol table
    for (auto& s : coff_syms) {
        m_os->write(&s, IMAGE_SIZEOF_SYMBOL);
    }

    // string table
    {
        DWORD len = (DWORD)strings.size() + sizeof(DWORD);
        m_os->write(&len, sizeof(len));
        m_os->write(strings.c_str(), strings.size());
    }

    return true;
}


template<class Arch>
bool PECOFFWriter<Arch>::writeExe(Context& ctx, IOutputStream& os)
{
    typedef PECOFFImpl<Arch> Impl;

    Impl::IMAGE_DOS_HEADER dos_header;
    Impl::IMAGE_NT_HEADERS nt_headers;
    Impl::IMAGE_EXPORT_DIRECTORY export_dir;
    std::vector<Impl::IMAGE_SECTION_HEADER> sec_headers;


    os.write(&dos_header, sizeof(dos_header));
    os.write(&nt_headers, sizeof(nt_headers));
    os.write(&export_dir, sizeof(export_dir));
    for(auto& sh : sec_headers) {
        m_os->write(&sh, IMAGE_SIZEOF_SECTION_HEADER);
    }

    return false;
}


template<class Arch>
bool PECOFFWriter<Arch>::writeDLL(Context& ctx, IOutputStream& os)
{
    typedef PECOFFImpl<Arch> Impl;

    return false;
}

template class PECOFFWriter<Arch_x86>;
template class PECOFFWriter<Arch_x64>;

} // namespace bg
