#include "pch.h"
#include "bgInternal.h"
#include "bgWriter.h"
#include "bgCOFF.h"

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
    typedef uint32 intptr;
    typedef IMAGE_NT_HEADERS32          IMAGE_NT_HEADERS;
    typedef IMAGE_OPTIONAL_HEADER32     IMAGE_OPTIONAL_HEADER;
};

template<>
struct PECOFFTypes<Arch_x64> : PECOFFTypesCommon
{
    typedef uint64 intptr;
    typedef IMAGE_NT_HEADERS64          IMAGE_NT_HEADERS;
    typedef IMAGE_OPTIONAL_HEADER64     IMAGE_OPTIONAL_HEADER;
};



template<class Arch>
struct PECOFFImpl : public PECOFFTypes<Arch>
{
    using typename PECOFFTypes<Arch>::intptr;

    WORD getMachineType();
    uint32 translateSectionFlags(SectionFlag flags, bool executable);
    uint32 translateRelocationType(RelocationType rel);
    uint32 buildSectionHeader(IMAGE_SECTION_HEADER& sh, Section& sec, uint32 pos, uint32 al, bool executable);
    uint32 align(uint32 pos, uint32 al);
    uint32 align(IOutputStream& os, uint32 pos, uint32 al);
};


template<class Arch>
class PECOFFWriter : public PECOFFImpl<Arch>
{
public:
    PECOFFWriter(PECOFFContext& ctx, IOutputStream& os);
    bool writeObj();
    bool writeExe();
    bool writeDLL();

private:
    bool writePE(bool is_dll);
    void buildDirective();
    void buildImportAddressTable(IMAGE_DATA_DIRECTORY& idd);
    void buildExportAddressTable(IMAGE_DATA_DIRECTORY& idd);


    PECOFFContext& m_ctx;
    IOutputStream& m_os;

    std::unique_ptr<Section> m_internal_section;
};

template<class Arch>
uint32 PECOFFImpl<Arch>::buildSectionHeader(IMAGE_SECTION_HEADER& sh, Section& section, uint32 pos, uint32 al, bool executable)
{
    auto& relocations = section.getRelocations();

    memcpy(sh.Name, section.getName(), 8);
    if (executable) {
        sh.Misc.VirtualSize = section.getSize();
        sh.VirtualAddress = section.getVirtualAddress();
        sh.SizeOfRawData = align(section.getSize(), al);
    }
    else {
        sh.SizeOfRawData = section.getSize();
    }
    sh.PointerToRawData = pos;
    sh.PointerToRelocations = relocations.empty() ? 0 : pos + section.getSize();
    sh.PointerToLinenumbers = 0;
    sh.NumberOfRelocations = (WORD)relocations.size();
    sh.NumberOfLinenumbers = 0;
    sh.Characteristics = translateSectionFlags(section.getFlags(), executable);

    return uint32(pos + section.getSize() + IMAGE_SIZEOF_RELOCATION * relocations.size());
}

template<> WORD PECOFFImpl<Arch_x86>::getMachineType() { return IMAGE_FILE_MACHINE_I386; }
template<> WORD PECOFFImpl<Arch_x64>::getMachineType() { return IMAGE_FILE_MACHINE_AMD64; }


template<class Arch>
uint32 PECOFFImpl<Arch>::translateSectionFlags(SectionFlag flags, bool executable)
{
    uint32 r = 0;
    if ((flags & SectionFlag::Code)) {
        r |= IMAGE_SCN_CNT_CODE;
        if (!executable) { r |= IMAGE_SCN_ALIGN_16BYTES; }
    }
    if ((flags & SectionFlag::IData)) {
        r |= IMAGE_SCN_CNT_INITIALIZED_DATA;
        if (!executable) { r |= IMAGE_SCN_ALIGN_16BYTES; }
    }
    if ((flags & SectionFlag::UData)) {
        r |= IMAGE_SCN_CNT_UNINITIALIZED_DATA;
        if (!executable) { r |= IMAGE_SCN_ALIGN_16BYTES; }
    }
    if ((flags & SectionFlag::Info)) {
        r |= IMAGE_SCN_LNK_INFO;
        if (!executable) { r |= IMAGE_SCN_ALIGN_1BYTES; }
    }
    if ((flags & SectionFlag::Read)) {
        r |= IMAGE_SCN_MEM_READ;
    }
    if ((flags & SectionFlag::Write)) {
        r |= IMAGE_SCN_MEM_WRITE;
    }
    if ((flags & SectionFlag::Execute)) {
        r |= IMAGE_SCN_MEM_EXECUTE;
    }
    if ((flags & SectionFlag::Shared)) {
        r |= IMAGE_SCN_MEM_SHARED;
    }
    if ((flags & SectionFlag::Remove)) {
        r |= IMAGE_SCN_LNK_REMOVE;
    }
    return r;
}

template<>
uint32 PECOFFImpl<Arch_x86>::translateRelocationType(RelocationType rel)
{
    switch (rel) {
    case RelocationType::ABS:       return IMAGE_REL_I386_ABSOLUTE;
    case RelocationType::REL32:     return IMAGE_REL_I386_REL32;
    case RelocationType::ADDR32:    return IMAGE_REL_I386_DIR32;
    case RelocationType::ADDR32NB:  return IMAGE_REL_I386_DIR32NB;
    case RelocationType::ADDR64:    break;
    }
    return 0;
}

template<>
uint32 PECOFFImpl<Arch_x64>::translateRelocationType(RelocationType rel)
{
    switch (rel) {
    case RelocationType::ABS:       return IMAGE_REL_AMD64_ABSOLUTE;
    case RelocationType::REL32:     return IMAGE_REL_AMD64_REL32;
    case RelocationType::ADDR32:    return IMAGE_REL_AMD64_ADDR32;
    case RelocationType::ADDR32NB:  return IMAGE_REL_AMD64_ADDR32NB;
    case RelocationType::ADDR64:    return IMAGE_REL_AMD64_ADDR64;
    }
    return 0;
}

template<class Arch>
uint32 PECOFFImpl<Arch>::align(uint32 pos, uint32 al)
{
    uint32 pad = al - (pos % al);
    return pos + pad;
}
template<class Arch>
uint32 PECOFFImpl<Arch>::align(IOutputStream& os, uint32 pos, uint32 al)
{
    uint32 pad = al - (pos % al);
    char z = 0;
    for (uint32 i = 0; i < pad; ++i) {
        os.write(&z, 1);
    }
    return pos + pad;
}


template<class Arch>
PECOFFWriter<Arch>::PECOFFWriter(PECOFFContext& ctx, IOutputStream& os)
    : m_ctx(ctx)
    , m_os(os)
{
}


// structure of COFF (.obj) file

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

template<class Arch>
bool PECOFFWriter<Arch>::writeObj()
{
    auto& sections = m_ctx.getSections();
    auto& symbols = m_ctx.getSymbolTable().getSymbols();
    auto& strings = m_ctx.getStringTable().getData();

    IMAGE_FILE_HEADER coff_header;
    std::vector<IMAGE_SECTION_HEADER> coff_sects;
    std::vector<std::vector<IMAGE_RELOCATION>> coff_rels;
    std::vector<IMAGE_SYMBOL> coff_syms;

    DWORD pos_sections = DWORD(
        IMAGE_SIZEOF_FILE_HEADER +
        IMAGE_SIZEOF_SECTION_HEADER * sections.size()
    );
    DWORD pos_symbols = pos_sections;
    for (auto& s : sections) {
        pos_symbols += s->getSize();
        pos_symbols += (DWORD)(IMAGE_SIZEOF_RELOCATION * s->getRelocations().size());
    }

    coff_header.Machine = getMachineType();
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
        if ((sym.flags & SymbolFlag::Static)) {
            isym.StorageClass = IMAGE_SYM_CLASS_STATIC;
        }
        if ((sym.flags & SymbolFlag::External)) {
            isym.StorageClass = IMAGE_SYM_CLASS_EXTERNAL;
        }
        isym.NumberOfAuxSymbols = 0;
    }

    // build section info
    for (size_t si = 0; si < sections.size(); ++si) {
        auto& section = *sections[si];
        pos_sections = buildSectionHeader(coff_sects[si], section, pos_sections, 1, false);

        // build relocation info
        auto& rels = section.getRelocations();
        auto& crels = coff_rels[si];
        crels.resize(rels.size());
        for (size_t ri = 0; ri < rels.size(); ++ri) {
            auto& rel = rels[ri];
            auto& coff = crels[ri];
            coff.DUMMYUNIONNAME.VirtualAddress = rel.addr;
            coff.SymbolTableIndex = rel.symbol_index;
            coff.Type = translateRelocationType(rel.type);
        }
    }


    // write actual data

    // file header
    m_os.write(&coff_header, IMAGE_SIZEOF_FILE_HEADER);

    // section headers
    for (auto& sh : coff_sects) {
        m_os.write(&sh, IMAGE_SIZEOF_SECTION_HEADER);
    }

    // section contents
    for (size_t si = 0; si < sections.size(); ++si) {
        auto& section = sections[si];
        m_os.write(section->getData(), section->getSize());
        for (auto& r : coff_rels[si]) {
            m_os.write(&r, IMAGE_SIZEOF_RELOCATION);
        }
    }

    // symbol table
    for (auto& s : coff_syms) {
        m_os.write(&s, IMAGE_SIZEOF_SYMBOL);
    }

    // string table
    {
        DWORD len = (DWORD)strings.size() + sizeof(DWORD);
        m_os.write(&len, sizeof(len));
        m_os.write(strings.c_str(), strings.size());
    }

    return true;
}


template<class Arch>
bool PECOFFWriter<Arch>::writeExe()
{
    return writePE(false);
}

template<class Arch>
bool PECOFFWriter<Arch>::writeDLL()
{
    return writePE(true);
}

// structure of PE (.exe, .dll) file

//--------------------------//
// IMAGE_DOS_HEADER         //
//--------------------------//
// IMAGE_NT_HEADER          //
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

template<class Arch>
bool PECOFFWriter<Arch>::writePE(bool is_dll)
{
    auto& sections = m_ctx.getSections();
    m_internal_section.reset(new Section(&m_ctx, ".intrnl", (uint32)sections.size(), SectionFlag::IData));
    size_t num_sections = sections.size() + 1;

    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS nt_headers;
    std::vector<IMAGE_SECTION_HEADER> sec_headers;
    IMAGE_DATA_DIRECTORY import_dir;
    IMAGE_DATA_DIRECTORY export_dir;

    memset(&dos_header, 0, sizeof(dos_header));
    memset(&nt_headers, 0, sizeof(nt_headers));
    sec_headers.resize(num_sections);

    uint32 section_align = 0x1000;
    uint32 file_align = 0x200; // must be 512 - 64k
    uint32 image_size = 0;
    uint32 entry_point = m_ctx.getSymbolTable().getVirtualAddress(m_ctx.getEntryPoint().c_str());
    uint32 pos_sections = uint32(
        sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS) + IMAGE_SIZEOF_SECTION_HEADER * num_sections);

    {
        // set virtual addresses
        uint32 pos = section_align;
        for (auto& s : sections) {
            if (s->getVirtualAddress() == 0) {
                s->setVirtualAddress(pos);
            }
            else {
                pos = s->getVirtualAddress();
            }
            pos += s->getSize() + (section_align - 1);
            pos &= ~(section_align - 1);
        }

        // build internal section data
        m_internal_section->setVirtualAddress(pos);
        buildImportAddressTable(import_dir);
        buildExportAddressTable(export_dir);
        pos += m_internal_section->getSize() + (section_align - 1);
        pos &= ~(section_align - 1);

        image_size = pos;
    }


    dos_header.e_magic = 0x5A4D; // "MZ"
    dos_header.e_lfanew = sizeof(IMAGE_DOS_HEADER);
    {
        auto& fh = nt_headers.FileHeader;
        fh.Machine = getMachineType();
        fh.NumberOfSections = (WORD)num_sections;
        fh.TimeDateStamp = 0;
        fh.PointerToSymbolTable = 0;
        fh.NumberOfSymbols = 0;
        fh.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER);

        fh.Characteristics = IMAGE_FILE_EXECUTABLE_IMAGE;
        if (is_dll) {
            fh.Characteristics |= IMAGE_FILE_DLL;
        }

        switch (getMachineType()) {
        case IMAGE_FILE_MACHINE_AMD64:
            fh.Characteristics |= IMAGE_FILE_LARGE_ADDRESS_AWARE;
            break;
        case IMAGE_FILE_MACHINE_I386:
            fh.Characteristics |= IMAGE_FILE_32BIT_MACHINE;
            break;
        }
    }
    {
        nt_headers.Signature = 0x4550; // "PE"

        auto& oh = nt_headers.OptionalHeader;
        switch (getMachineType()) {
        case IMAGE_FILE_MACHINE_AMD64:
            oh.Magic = 0x020B;
            break;
        case IMAGE_FILE_MACHINE_I386:
            oh.Magic = 0x010B;
            break;
        }

        oh.SizeOfCode;              // can be omitted
        oh.SizeOfInitializedData;   // 
        oh.SizeOfUninitializedData; // 
        oh.AddressOfEntryPoint = entry_point;
        oh.BaseOfCode = section_align;
        oh.ImageBase = (intptr)m_ctx.getBaseAddress();
        oh.SectionAlignment = section_align;
        oh.FileAlignment = file_align;
        oh.MajorOperatingSystemVersion = 0x0006;
        oh.MinorOperatingSystemVersion = 0x0000;
        oh.MajorSubsystemVersion = 0x0006;
        oh.MinorSubsystemVersion = 0x0000;
        oh.SizeOfImage = image_size;
        oh.SizeOfHeaders = align(pos_sections, file_align);

        switch (m_ctx.getSubsystem()) {
        case Subsystem::CUI:
            oh.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
            break;
        case Subsystem::GUI:
            oh.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
            break;
        }
        oh.DllCharacteristics = IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE;
        oh.SizeOfStackReserve = 0x100000;
        oh.SizeOfStackCommit  = 0x1000;
        oh.SizeOfHeapReserve  = 0x100000;
        oh.SizeOfHeapCommit   = 0x1000;
        oh.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

        oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT] = export_dir;
        oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = import_dir;
    }

    // build section info
    {
        uint32 spos = align(pos_sections, file_align);
        for (size_t si = 0; si < sections.size(); ++si) {
            spos = buildSectionHeader(sec_headers[si], *sections[si], spos, file_align, true);
            spos = align(spos, file_align);
        }
        spos = buildSectionHeader(sec_headers.back(), *m_internal_section, spos, file_align, true);
    }

    m_os.write(&dos_header, sizeof(dos_header));
    m_os.write(&nt_headers, sizeof(nt_headers));
    for (auto& sh : sec_headers) {
        m_os.write(&sh, IMAGE_SIZEOF_SECTION_HEADER);
    }
    uint32 wpos = align(m_os, pos_sections, file_align);

    for (auto& s : sections) {
        m_os.write(s->getData(), s->getSize());
        wpos = align(m_os, wpos + s->getSize(), file_align);
    }
    {
        m_os.write(m_internal_section->getData(), m_internal_section->getSize());
        wpos = align(m_os, wpos + m_internal_section->getSize(), file_align);
    }

    return true;
}


template<class Arch>
void PECOFFWriter<Arch>::buildDirective()
{
}

template<class Arch>
void PECOFFWriter<Arch>::buildImportAddressTable(IMAGE_DATA_DIRECTORY& idd)
{
    memset(&idd, 0, sizeof(IMAGE_DATA_DIRECTORY));
    // todo
}

template<class Arch>
void PECOFFWriter<Arch>::buildExportAddressTable(IMAGE_DATA_DIRECTORY& idd)
{
    auto& symtab = m_ctx.getSymbolTable();
    auto& exports = m_ctx.getDLLExports();
    auto& filename = m_ctx.getFileName();
    size_t size_nametable = filename.size() + 1;
    for(auto& n : exports) {
        size_nametable += n.size() + 1;
    }

    // reserve data spaces
    uint32 va = m_internal_section->getVirtualAddress();
    uint32 pos_ied      = m_internal_section->addData(nullptr, sizeof(IMAGE_EXPORT_DIRECTORY));
    uint32 pos_funcs    = m_internal_section->addData(nullptr, sizeof(intptr) * exports.size());
    uint32 pos_names    = m_internal_section->addData(nullptr, sizeof(uint32) * exports.size());
    uint32 pos_ordinals = m_internal_section->addData(nullptr, sizeof(uint16) * exports.size());
    uint32 pos_nametable= m_internal_section->addData(nullptr, size_nametable);

    // pointers to actual data
    auto* ied = (IMAGE_EXPORT_DIRECTORY*)(m_internal_section->getData() + pos_ied);
    auto* funcs = (intptr*)(m_internal_section->getData() + pos_funcs);
    auto* names = (uint32*)(m_internal_section->getData() + pos_names);
    auto* ordinals = (uint16*)(m_internal_section->getData() + pos_ordinals);
    auto* nametable = (char*)(m_internal_section->getData() + pos_nametable);

    // build export data
    {
        uint32 npos = 0;
        uint32 i = 0;
        {
            memcpy(nametable + npos, filename.c_str(), filename.size() + 1);
            npos += uint32(filename.size() + 1);
        }
        for (auto& n : exports) {
            funcs[i] = symtab.getVirtualAddress(n.c_str());
            names[i] = va + pos_nametable + npos;
            ordinals[i] = (uint16)i;
            memcpy(nametable + npos, n.c_str(), n.size() + 1);
            npos += uint32(n.size() + 1);
        }
    }

    // build IMAGE_EXPORT_DIRECTORY
    memset(ied, 0, sizeof(IMAGE_EXPORT_DIRECTORY));
    ied->Base = 1;
    ied->Name = va + pos_nametable;
    ied->NumberOfFunctions = (uint32)exports.size();
    ied->NumberOfNames = (uint32)exports.size();
    ied->AddressOfFunctions = va + pos_funcs;
    ied->AddressOfNames = va + pos_names;
    ied->AddressOfNameOrdinals = va + pos_ordinals;

    idd.VirtualAddress = va + pos_ied;
    idd.Size = m_internal_section->getSize() - pos_ied;
}



template<class Arch> bool PECOFFWriteObj(PECOFFContext& ctx, IOutputStream& os)
{
    PECOFFWriter<Arch> writer(ctx, os);
    return writer.writeObj();
}
template<class Arch> bool PECOFFWriteExe(PECOFFContext& ctx, IOutputStream& os)
{
    PECOFFWriter<Arch> writer(ctx, os);
    return writer.writeExe();
}
template<class Arch> bool PECOFFWriteDLL(PECOFFContext& ctx, IOutputStream& os)
{
    PECOFFWriter<Arch> writer(ctx, os);
    return writer.writeDLL();
}


template bool PECOFFWriteObj<Arch_x86>(PECOFFContext& ctx, IOutputStream& os);
template bool PECOFFWriteExe<Arch_x86>(PECOFFContext& ctx, IOutputStream& os);
template bool PECOFFWriteDLL<Arch_x86>(PECOFFContext& ctx, IOutputStream& os);
template bool PECOFFWriteObj<Arch_x64>(PECOFFContext& ctx, IOutputStream& os);
template bool PECOFFWriteExe<Arch_x64>(PECOFFContext& ctx, IOutputStream& os);
template bool PECOFFWriteDLL<Arch_x64>(PECOFFContext& ctx, IOutputStream& os);

} // namespace bg
