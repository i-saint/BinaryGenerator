#pragma once
namespace bg {

struct String
{
public:
    String();
    String(StringTable& t, uint32_t a);
    const char* str() const;

    StringTable  *table;
    uint32_t     rva;
};
bool operator==(const String&a, const String& b);
bool operator<(const String&a, const String& b);

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
    std::map<std::string, String> m_entries;
};

} // namespace bg
