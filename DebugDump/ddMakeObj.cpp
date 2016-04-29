#include "pch.h"
#include "DebugDump.h"
#include "../BinaryGenerator.h"
namespace dd {

static void WriteObj_COFF_x86(std::ostream& os, Symbols& syms)
{
    bg::Context ctx;

    ctx.write(os, bg::Format_COFF_x86);
}

static void WriteObj_COFF_x86_64(std::ostream& os, Symbols& syms)
{
    std::vector<uint64> jumptable;
    jumptable.resize(syms.size());

    bg::Context ctx;
    bg::Section *section = ctx.createSection(".textx", bg::SectionType_TextX);
    uint8 code[14] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (auto& sym : syms) {
        section->addExternalSymbol(code, sizeof(code), sym.name.c_str());
    }
    ctx.write(os, bg::Format_COFF_x86);
}

void WriteObj(std::ostream& os, Symbols& syms, bg::Format fmt)
{
    switch (fmt) {
    case bg::Format_COFF_x86:
        WriteObj_COFF_x86(os, syms);
        break;
    case bg::Format_COFF_x64:
        WriteObj_COFF_x86_64(os, syms);
        break;
    }
}

} // namespace dd
