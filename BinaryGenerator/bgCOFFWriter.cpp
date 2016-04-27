#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"
#include "bgSection.h"
#include "bgCOFF.h"
#include "bgCOFFWriter.h"

namespace bg {

COFFWriter::COFFWriter()
    : m_ctx()
    , m_os()
    , m_written()
{

}

COFFWriter::~COFFWriter()
{

}

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
// section data * n         //
//                          //
//                          //
//                          //
//--------------------------//

bool COFFWriter::write(Context& ctx, std::ostream& os)
{
    m_ctx = &ctx;
    m_os = &os;

    auto& sections = m_ctx->getSections();
    auto& symbols = m_ctx->getSymbolTable().getSymbols();
    auto& strings = m_ctx->getStringTable().getData();

    size_t pos_symbols =
        sizeof(IMAGE_FILE_HEADER) + 
        sizeof(IMAGE_SECTION_HEADER) * sections.size();
    size_t pos_sections = pos_symbols + 
        sizeof(IMAGE_SYMBOL) * symbols.size() +
        strings.size();

    // write headers
    {
        IMAGE_FILE_HEADER ifh;
        ifh.Machine = IMAGE_FILE_MACHINE_AMD64;
        ifh.NumberOfSections = (WORD)sections.size();
        ifh.TimeDateStamp = 0;
        ifh.PointerToSymbolTable = (DWORD)pos_symbols;
        ifh.NumberOfSymbols = (DWORD)symbols.size();
        ifh.SizeOfOptionalHeader = 0;
        ifh.Characteristics = 0;
        os.write((char*)&ifh, sizeof(ifh));
    }
    for (auto& section : sections) {
        IMAGE_SECTION_HEADER ish;
        memcpy(ish.Name, section->getName(), 8);
        ish.VirtualAddress;
        ish.SizeOfRawData;
        ish.PointerToRawData;
        ish.PointerToRelocations;
        ish.PointerToLinenumbers;
        ish.NumberOfRelocations;
        ish.NumberOfLinenumbers;
        ish.Characteristics;
        os.write((char*)&ish, sizeof(ish));
    }
    for (auto& sym : symbols) {
        IMAGE_SYMBOL s;
        // 
        os.write((char*)&s, sizeof(s));
    }
    {
        os.write(strings.c_str(), strings.size());
    }


    return false;
}

} // namespace bg
