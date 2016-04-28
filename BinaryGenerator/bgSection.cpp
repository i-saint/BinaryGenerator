#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"
#include "bgSection.h"

namespace bg {

Section::Section(Context *ctx, const char *name, uint32_t index, uint32_t flags)
    : m_ctx(ctx)
    , m_index(index)
    , m_flags(flags)
{
    memset(m_name, 0, sizeof(m_name));
    strncpy(m_name, name, 8);

}

Symbol Section::addSymbol(const void *data_, size_t len, const char *name, uint32_t flags)
{
    auto pos = (uint32_t)m_data.size();
    auto *data = (const char*)data_;
    m_data.insert(m_data.end(), data, data + len);

    auto ret = Symbol(this, pos, m_ctx->getStringTable().addString(name), flags);
    return m_ctx->getSymbolTable().insert(ret);
}

Symbol Section::addExternalSymbol(const char *name)
{
    auto ret = Symbol(nullptr, 0, m_ctx->getStringTable().addString(name));
    return m_ctx->getSymbolTable().insert(ret);
}

Relocation Section::addRelocation(uint32_t pos, const char *name, RelocationType type)
{
    auto ret = Relocation();
    ret.section = this;
    ret.rva = pos;
    ret.sym = addExternalSymbol(name);
    ret.type = type;
    m_reloc.emplace_back(ret);
    return ret;
}

const char* Section::getName() const { return m_name; }
uint32_t Section::getIndex() const { return m_index; }
uint32_t Section::getFlags() const { return m_flags; }
const std::string& Section::getData() const { return m_data; }
Section::Relocations& Section::getRelocations() { return m_reloc; }

} // namespace bg
