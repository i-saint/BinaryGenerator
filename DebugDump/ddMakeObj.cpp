#include "pch.h"
#include "DebugDump.h"
#define bgStaticLink
#include "../BinaryGenerator.h"

namespace dd {

static std::string MakeExportDirective(Symbols& syms)
{
    std::string ret;
    char buf[2048];
    for (auto& s : syms) {
        sprintf(buf, "/EXPORT:%s ", s.name.c_str());
        ret += buf;
    }
    return ret;
}

static void WriteObj_COFF_x86(std::ostream& os, Symbols& syms)
{
    typedef uint32 intptr;

    uint32 num_syms = (uint32)syms.size();
    std::vector<intptr> jumptable;
    jumptable.resize(num_syms);
    for (size_t i = 0; i < num_syms; ++i) {
        jumptable[i] = syms[i].addr;
    }

    bg::IContext *ctx = bg::CreateContext();
    bg::ISection *directive = ctx->createSection(".drectve", bg::SectionType_Info);
    bg::ISection *text = ctx->createSection(".textx", bg::SectionType_TextX);

    uint32 code_len = 6 * num_syms;
    uint8 code[6] = { 0xff, 0x25, 0x00, 0x00, 0x00, 0x00 };
    for (auto& sym : syms) {
        text->addExternalSymbol(code, sizeof(code), sym.name.c_str());
    }
    text->addExternalSymbol(0, "_g_jumptable");
    text->addExternalSymbol(&jumptable[0], jumptable.size() * sizeof(intptr), "_g_addrtable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "_g_num_symbols");

    auto exports = MakeExportDirective(syms);
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->write(os, bg::Format_COFF_x86);
    ctx->release();
}

static void WriteObj_COFF_x86_64(std::ostream& os, Symbols& syms)
{
    typedef uint64 intptr;

    uint32 num_syms = (uint32)syms.size();
    std::vector<intptr> jumptable;
    jumptable.resize(num_syms);
    for (size_t i = 0; i < num_syms; ++i) {
        jumptable[i] = syms[i].addr;
    }

    bg::IContext *ctx = bg::CreateContext();
    bg::ISection *directive = ctx->createSection(".drectve", bg::SectionType_Info);
    bg::ISection *text = ctx->createSection(".textx", bg::SectionType_TextX);

    uint32 code_len = 6 * num_syms;
    uint8 code[6] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00 };
    for (size_t si = 0; si < syms.size(); ++si) {
        auto& sym = syms[si];
        bg::Symbol s = text->addExternalSymbol(code, sizeof(code), sym.name.c_str());
        uint32 *offset = (uint32*)(text->getData() + s.addr + 2);
        *offset = (uint32)(code_len - (si + 1) * 6 + sizeof(intptr) * si);
    }
    text->addExternalSymbol(0, "g_jumptable");
    text->addExternalSymbol(&jumptable[0], jumptable.size() * sizeof(intptr), "g_addrtable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "g_num_symbols");

    auto exports = MakeExportDirective(syms);
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->write(os, bg::Format_COFF_x64);
    ctx->release();
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
