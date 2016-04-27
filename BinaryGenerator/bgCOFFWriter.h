#pragma once
namespace bg {

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

typedef COFFWriter COFFWriterx86;
typedef COFFWriter COFFWriterx86_64;

} // namespace bg
