#include "pch.h"
#include "bgInternal.h"

namespace bg {

bool operator==(const String&a, const String& b)
{
    return a.addr == b.addr;
}



StringTable::StringTable(Context *ctx)
    : m_ctx(ctx)
{
}

StringTable::StringTable(Context *ctx, const StringTable& base)
    : m_ctx(ctx)
    , m_table(base.m_table)
    , m_entries(base.m_entries)
{
}

const String& StringTable::addString(const char *str)
{
    auto i = m_entries.find(str);
    if (i != m_entries.end()) {
        return i->second;
    }
    else {
        auto addr = (uint32)m_table.size() + 4;
        m_table.insert(m_table.end(), str, str + strlen(str) + 1);

        String tmp = {addr};
        auto e = m_entries.insert(std::make_pair(str, tmp));
        return e.first->second;
    }
}

const char* StringTable::getString(uint32 i)
{
    return &m_table[i - 4];
}

const std::string& StringTable::getData() const { return m_table; }


} // namespace bg
