#pragma once
namespace bg {

bool operator==(const String&a, const String& b);

class StringTable
{
public:
    StringTable(Context *ctx);
    StringTable(Context *ctx, const StringTable& base);
    const String& addString(const char *str);
    const char* get(uint32 i);

public:
    const std::string& getData() const;

private:
    using Entries = std::map<std::string, String>;

    Context *m_ctx;
    std::string m_table;
    Entries m_entries;
};

} // namespace bg
