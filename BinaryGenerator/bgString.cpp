#include "pch.h"
#include "bgInternal.h"

namespace bg {

const char* String::str() const
{
    return table == nullptr ? nullptr : table->get(addr);
}

bool operator==(const String&a, const String& b)
{
    return std::strcmp(a.str(), b.str()) == 0;
}
bool operator==(const String&a, const char* b)
{
    return std::strcmp(a.str(), b) == 0;
}
bool operator<(const String&a, const String& b)
{
    return std::strcmp(a.str(), b.str()) < 0;
}



StringTable::StringTable(Context *ctx)
    : m_ctx(ctx)
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

        String tmp = {this, addr};
        auto e = m_entries.insert(std::make_pair(str, tmp));
        return e.first->second;
    }
}

const char* StringTable::get(uint32 i)
{
    return &m_table[i];
}

const std::string& StringTable::getData() const { return m_table; }


} // namespace bg
