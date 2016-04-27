#ifndef bg_Section_h
#define bg_Section_h
namespace bg {


class Section
{
public:
    Section();
    size_t insert(const void *data, size_t len);

private:
    std::vector<char> m_data;
};

} // namespace bg
#endif // bg_Section_h
