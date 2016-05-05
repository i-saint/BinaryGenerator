#include "pch.h"
#include "DebugExport.h"

namespace de {

std::string Context::makeExportDirective() const
{
    std::string ret;
    char buf[2048];
    for (auto& s : m_syms) {
        sprintf(buf, "/EXPORT:%s ", getName(s));
        ret += buf;
    }
    return ret;
}

void Context::generateObj_COFF_x86(const char *out_path) const
{
    typedef uint32 intptr;

    auto *ctx = bg::CreatePECOFFContext(bg::Architecture::x86);
    auto *directive = ctx->createSection(".drectve", bg::SectionFlag::Info);
    auto *text = ctx->createSection(".textx", bg::SectionFlag::TextXSection);

    uint32 num_syms = (uint32)m_syms.size();
    uint8 code[10] = { 0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (size_t si = 0; si < m_syms.size(); ++si) {
        auto& sym = m_syms[si];
        bg::Symbol s = text->addExternalSymbol(code, sizeof(code), getName(sym));
        intptr *addr = (intptr*)(text->getData() + s.addr + 6);
        *addr = sym.addr;
    }
    text->addExternalSymbol(0, "_g_jumptable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "_g_num_symbols");

    auto exports = makeExportDirective();
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->writeObj(out_path);
    ctx->release();
}

void Context::generateObj_COFF_x64(const char *out_path) const
{
    typedef uint64 intptr;

    auto *ctx = bg::CreatePECOFFContext(bg::Architecture::x64);
    auto *directive = ctx->createSection(".drectve", bg::SectionFlag::Info);
    auto *text = ctx->createSection(".textx", bg::SectionFlag::TextXSection);

    uint32 num_syms = (uint32)m_syms.size();
    uint8 code[14] = {0xff, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    for (size_t si = 0; si < m_syms.size(); ++si) {
        auto& sym = m_syms[si];
        bg::Symbol s = text->addExternalSymbol(code, sizeof(code), getName(sym));
        intptr *addr = (intptr*)(text->getData() + s.addr + 6);
        *addr = sym.addr;
    }
    text->addExternalSymbol(0, "g_jumptable");
    text->addExternalSymbol(&num_syms, sizeof(num_syms), "g_num_symbols");

    auto exports = makeExportDirective();
    directive->addStaticSymbol(exports.c_str(), exports.size(), ".drectve");

    ctx->writeObj(out_path);
    ctx->release();
}

void Context::generateObj(const char *out_path, bg::Format fmt, bg::Architecture arch)
{
    if (fmt == bg::Format::PECOFF) {
        switch (arch) {
        case bg::Architecture::x86:
            generateObj_COFF_x86(out_path);
            break;
        case bg::Architecture::x64:
            generateObj_COFF_x64(out_path);
            break;
        }
    }
    else if (fmt == bg::Format::ELF) {
        // elf is not implemented yet
    }
}

} // namespace de
