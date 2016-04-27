#include "pch.h"
#include "bgContext.h"
#include "bgString.h"

namespace bg {

String::String() : m_table(), m_rva()
{}

String::String(StringTable& table, uint32_t rva)
    : m_table(&table), m_rva(rva)
{}

const char* String::str() const
{
    return m_table == nullptr ? nullptr : m_table->get(m_rva);
}

bool operator==(const String&a, const String& b)
{
    return std::strcmp(a.str(), b.str()) == 0;
}
bool operator<(const String&a, const String& b)
{
    return std::strcmp(a.str(), b.str()) < 0;
}

bool CharCompare::operator()(const char *a, const char *b) const
{
    return std::strcmp(a, b) < 0;
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
        auto pos = (uint32_t)m_table.size();
        m_table.insert(m_table.end(), str, str + strlen(str) + 1);
        auto e = m_entries.insert(std::make_pair(&m_table[pos], String(*this, pos)));
        return e.first->second;
    }
}

const char* StringTable::get(uint32_t i)
{
    return &m_table[i];
}

const std::string& StringTable::getTable() const { return m_table; }


} // namespace bg
