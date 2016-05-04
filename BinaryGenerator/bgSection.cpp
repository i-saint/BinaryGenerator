#include "pch.h"
#include "bgInternal.h"

namespace bg {

Section::Section(Context *ctx, const char *name, uint32 index, SectionFlag flags)
    : m_ctx(ctx)
    , m_index(index)
    , m_flags(flags)
    , m_virtual_addr()
{
    memset(m_name, 0, sizeof(m_name));
    strncpy(m_name, name, 8);

}

Section::~Section()
{
}

uint32 Section::addData(const void *data, size_t len)
{
    auto pos = (uint32)m_data.size();
    if (data) {
        m_data.insert(m_data.end(), (char*)data, (char*)data + len);
    }
    else {
        m_data.resize(m_data.size() + len);
    }
    return pos;
}

Symbol Section::addSymbol(const void *data, size_t len, const char *name, SymbolFlag flags)
{
    return addSymbol(addData(data, len), name, flags);
}

Symbol Section::addSymbol(uint32 addr, const char *name, SymbolFlag flags)
{
    auto ret = Symbol();
    ret.section = this;
    ret.index = 0;
    ret.addr = addr;
    ret.flags = flags;
    ret.name = m_ctx->getStringTable().addString(name);
    return m_ctx->getSymbolTable().addSymbol(ret);
}

Symbol Section::addUndefinedSymbol(const char *name)
{
    auto ret = Symbol();
    ret.section = nullptr;
    ret.index = 0;
    ret.addr = 0;
    ret.flags = SymbolFlag::External;
    ret.name = m_ctx->getStringTable().addString(name);
    return m_ctx->getSymbolTable().addSymbol(ret);
}

Relocation Section::addRelocation(uint32 pos, const char *sym_name, RelocationType type)
{
    return addRelocation(pos, addUndefinedSymbol(sym_name).index, type);
}

Relocation Section::addRelocation(uint32 pos, uint32 symbol_index, RelocationType type)
{
    auto ret = Relocation();
    ret.section = this;
    ret.addr = pos;
    ret.symbol_index = symbol_index;
    ret.type = type;
    m_reloc.emplace_back(ret);
    return ret;
}

const char* Section::getName() const { return m_name; }
uint32 Section::getIndex() const { return m_index; }
SectionFlag Section::getFlags() const { return m_flags; }
uint32 Section::getVirtualAddress() const { return m_virtual_addr; }

uint32 Section::getSize() const { return (uint32)m_data.size(); }
char* Section::getData() { return m_data.empty() ? nullptr : &m_data[0]; }

void Section::setVirtualAddress(uint32 va) { m_virtual_addr = va; }

Section::Relocations& Section::getRelocations() { return m_reloc; }

} // namespace bg
