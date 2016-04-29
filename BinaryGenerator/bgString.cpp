#include "pch.h"
#include "bgFoundation.h"
#include "bgContext.h"
#include "bgString.h"

namespace bg {

String::String() : table(), rva()
{}

String::String(StringTable& t, uint32_t a)
    : table(&t), rva(a)
{}

const char* String::str() const
{
    return table == nullptr ? nullptr : table->get(rva);
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
        auto pos = (uint32_t)m_table.size() + 4;
        m_table.insert(m_table.end(), str, str + strlen(str) + 1);
        auto e = m_entries.insert(std::make_pair(str, String(*this, pos)));
        return e.first->second;
    }
}

const char* StringTable::get(uint32_t i)
{
    return &m_table[i];
}

const std::string& StringTable::getData() const { return m_table; }


} // namespace bg
