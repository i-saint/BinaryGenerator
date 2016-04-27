#pragma once
namespace bg {

class String
{
public:
    String();
    String(StringTable& table, uint32_t rva);
    const char* str() const;

private:
    StringTable* m_table;
    uint32_t     m_rva;
};
bool operator==(const String&a, const String& b);
bool operator<(const String&a, const String& b);

struct CharCompare
{
    bool operator()(const char *a, const char *b) const;
};

class StringTable
{
public:
    StringTable(Context *ctx);
    const String& addString(const char *str);
    const char* get(uint32_t i);

public:
    const std::string& getData() const;

private:
    Context *m_ctx;
    std::string m_table;
    std::map<const char*, String, CharCompare> m_entries;
};

} // namespace bg
