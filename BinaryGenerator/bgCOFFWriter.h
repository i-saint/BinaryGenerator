#pragma once
namespace bg {

template<class Traits>
class COFFWriter
{
public:
    COFFWriter();
    bool write(Context& ctx, std::ostream& os);

protected:
    Context *m_ctx;
    std::ostream *m_os;
};

} // namespace bg
