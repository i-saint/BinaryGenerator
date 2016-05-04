#include "pch.h"
#include "DebugDump.h"
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

static void WriteObj_COFF_x86(bg::IOutputStream& os, Symbols& syms)
{
    typedef uint32 intptr;

    auto *ctx = bg::CreatePECOFFContext(bg::Architecture::x86);
    auto *directive = ctx->createSection(".drectve", bg::SectionFlag::Info);
    auto *text = ctx->createSection(".textx", bg::SectionFlag::TextXSection);

    uint32 num_syms = (uint32)syms.size();
    uint8 code[10] = { 0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (size_t si = 0; si < syms.size(); ++si) {
        auto& sym = syms[si];
        bg::Symbol s = text->addExternalSymbol(code, sizeof(code), sym.name.c_str());
        intptr *addr = (intptr*)(text->getData() + s.addr + 6);
        *addr = sym.addr;
    }
    text->addExternalSymbol(0, "_g_jumptable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "_g_num_symbols");

    auto exports = MakeExportDirective(syms);
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->writeObj(os);
    ctx->release();
}

static void WriteObj_COFF_x64(bg::IOutputStream& os, Symbols& syms)
{
    typedef uint64 intptr;

    auto *ctx = bg::CreatePECOFFContext(bg::Architecture::x64);
    auto *directive = ctx->createSection(".drectve", bg::SectionFlag::Info);
    auto *text = ctx->createSection(".textx", bg::SectionFlag::TextXSection);

    uint32 num_syms = (uint32)syms.size();
    uint8 code[14] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (size_t si = 0; si < syms.size(); ++si) {
        auto& sym = syms[si];
        bg::Symbol s = text->addExternalSymbol(code, sizeof(code), sym.name.c_str());
        intptr *addr = (intptr*)(text->getData() + s.addr + 6);
        *addr = sym.addr;
    }
    text->addExternalSymbol(0, "g_jumptable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "g_num_symbols");

    auto exports = MakeExportDirective(syms);
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->writeObj(os);
    ctx->release();
}

void WriteObj(bg::IOutputStream& os, Symbols& syms, bg::Architecture arch)
{
    switch (arch) {
    case bg::Architecture::x86:
        WriteObj_COFF_x86(os, syms);
        break;
    case bg::Architecture::x64:
        WriteObj_COFF_x64(os, syms);
        break;
    }
}

} // namespace dd
