#include "pch.h"
#include "Context.h"
#include "String.h"
#include "Section.h"

namespace bg {

Section::Section()
{
}

size_t Section::insert(const void *data_, size_t len)
{
    auto ret = m_data.size();
    auto *data = (const char*)data_;
    m_data.insert(m_data.end(), data, data + len);
    return ret;
}

} // namespace bg
