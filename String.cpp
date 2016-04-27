#include "pch.h"
#include "Context.h"
#include "String.h"

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



StringTable::StringTable()
{
}

String StringTable::make(const char *str)
{
    auto i = m_table.find(str);
    if (i != m_table.end()) {
        return String(*this, i->second);
    }
    else {
        size_t pos = m_pool.size();
        m_pool.insert(m_pool.end(), str, str + strlen(str) + 1);
        return String(*this, (uint32_t)pos);
    }
}

String StringTable::make(uint32_t i)
{
    return String(*this, i);
}

const char* StringTable::get(uint32_t i)
{
    return &m_pool[i];
}

} // namespace bg
