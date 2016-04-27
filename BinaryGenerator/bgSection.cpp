#include "pch.h"
#include "bgContext.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgRelocation.h"
#include "bgSection.h"

namespace bg {

Section::Section(Context *ctx)
    : m_ctx(ctx)
    , m_flags()
{
}

Symbol Section::addSymbol(const void *data_, size_t len, const char *name)
{
    auto pos = (uint32_t)m_data.size();
    auto *data = (const char*)data_;
    m_data.insert(m_data.end(), data, data + len);

    auto ret = Symbol(this, pos, m_ctx->getStringTable().addString(name));
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
    return ret;
}

void* Section::ptr(uint32_t pos /*= 0*/)
{
    return &m_data[pos];
}

} // namespace bg
