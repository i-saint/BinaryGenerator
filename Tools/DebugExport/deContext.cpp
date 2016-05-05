#include "pch.h"
#include "DebugExport.h"

namespace de {

Context::Context()
{
}

Context::~Context()
{
}

void Context::readSymbols(const char *in_path)
{

}

void Context::dumpSymbols(const char *out_path)
{
}


void Context::generateDLL(const char *out_path, bg::Format fmt, bg::Architecture arch)
{
}

void Context::modifyExecutable(const char *target_path, bool backup)
{
}

size_t Context::getNumSymbols() const { return m_syms.size(); }
Symbol* Context::getSymbol() { return m_syms.empty() ? nullptr : &m_syms[0]; }
const char* Context::getName(const Symbol& sym) const { return &m_string_table[sym.addr]; }

uint32 Context::addName(const char *name)
{
    size_t pos = m_string_table.size();
    m_string_table += name;
    m_string_table += '\0';
    return (uint32)pos;
}

uint32 Context::addSymbol(const Symbol& sym)
{
    size_t pos = m_syms.size();
    m_syms.emplace_back(sym);
    return (uint32)pos;
}

} // namespace de
