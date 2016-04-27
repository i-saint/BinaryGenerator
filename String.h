#ifndef bg_String_h
#define bg_String_h
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
    StringTable();
    String make(const char *str);
    String make(uint32_t i);
    const char* get(uint32_t i);

private:
    std::string m_pool;
    std::map<const char*, uint32_t, CharCompare> m_table;
};

} // namespace bg
#endif // bg_String_h
