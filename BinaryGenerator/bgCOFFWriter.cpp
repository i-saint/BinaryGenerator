#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"
#include "bgSection.h"
#include "bgCOFF.h"
#include "bgCOFFWriter.h"


// structure of COFF file

//--------------------------//
// IMAGE_FILE_HEADER        //
//--------------------------//
//                          //
// IMAGE_SECTION_HEADER x n //
//                          //
//--------------------------//
//                          //
// IMAGE_SYMBOL x n         //
//                          //
//--------------------------//
// string table             //
//--------------------------//
//                          //
//                          //
//                          //
// section data x n         //
//                          //
//                          //
// (relocation info)        //
//--------------------------//

namespace bg {

template<class T>
COFFWriter<T>::COFFWriter()
    : m_ctx()
    , m_os()
    , m_written()
{

}

template<class T>
COFFWriter<T>::~COFFWriter()
{

}

template<class Traits> struct COFFImpl;

template<>
struct COFFImpl<Traits_x86>
{
    static const WORD Machine = IMAGE_FILE_MACHINE_I386;
};

template<>
struct COFFImpl<Traits_x86_64>
{
    static const WORD Machine = IMAGE_FILE_MACHINE_AMD64;
};

template<class T>
bool COFFWriter<T>::write(Context& ctx, std::ostream& os)
{
    typedef COFFImpl<T> ImplT;

    m_ctx = &ctx;
    m_os = &os;

    auto& sections = m_ctx->getSections();
    auto& symbols = m_ctx->getSymbolTable().getSymbols();
    auto& strings = m_ctx->getStringTable().getData();

    IMAGE_FILE_HEADER file_header;
    std::vector<IMAGE_SECTION_HEADER> section_headers;
    std::vector<std::vector<IMAGE_RELOCATION>> image_relocations;
    std::vector<IMAGE_SYMBOL> image_symbols;

    size_t pos_symbols =
        sizeof(IMAGE_FILE_HEADER) + 
        sizeof(IMAGE_SECTION_HEADER) * sections.size();
    size_t pos_sections = pos_symbols + 
        sizeof(IMAGE_SYMBOL) * symbols.size() +
        strings.size();

    file_header.Machine = ImplT::Machine;
    file_header.NumberOfSections = (WORD)sections.size();
    file_header.TimeDateStamp = 0;
    file_header.PointerToSymbolTable = (DWORD)pos_symbols;
    file_header.NumberOfSymbols = (DWORD)symbols.size();
    file_header.SizeOfOptionalHeader = 0;
    file_header.Characteristics = 0;

    section_headers.resize(sections.size());
    image_relocations.resize(sections.size());
    image_symbols.resize(symbols.size());

    for (size_t i = 0; i < sections.size(); ++i) {
        auto& section = sections[i];
        auto& data = section->getData();
        auto& rels = section->getRelocations();

        auto& sh = section_headers[i];
        auto& ir = image_relocations[i];

        memcpy(sh.Name, section->getName(), 8);
        sh.VirtualAddress;
        sh.SizeOfRawData;
        sh.PointerToRawData;
        sh.PointerToRelocations;
        sh.PointerToLinenumbers = 0;
        sh.NumberOfRelocations;
        sh.NumberOfLinenumbers = 0;
        sh.Characteristics;

        ir.resize(rels.size());
    }


    // write headers
    {
        m_os->write((char*)&file_header, sizeof(file_header));
    }
    for (auto& sh : section_headers) {
        m_os->write((char*)&sh, sizeof(sh));
    }
    for (auto& s : image_symbols) {
        m_os->write((char*)&s, sizeof(s));
    }
    {
        m_os->write(strings.c_str(), strings.size());
    }


    // write section contents
    for (auto& section : sections) {
        auto& data = section->getData();
        auto& rels = section->getRelocations();
        if (!data.empty()) {
            m_os->write(data.c_str(), data.size());
        }
        if (!rels.empty()) {
            m_os->write((char*)&rels[0], sizeof(IMAGE_RELOCATION) * rels.size());
        }
    }

    return true;
}

template class COFFWriter<Traits_x86>;
template class COFFWriter<Traits_x86_64>;

} // namespace bg
