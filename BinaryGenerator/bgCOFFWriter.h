#pragma once
namespace bg {

template<class Traits>
class COFFWriter
{
public:
    COFFWriter();
    virtual ~COFFWriter();
    virtual bool write(Context& ctx, std::ostream& os);

protected:
    Context *m_ctx;
    std::ostream *m_os;
    size_t m_written;
};

} // namespace bg
