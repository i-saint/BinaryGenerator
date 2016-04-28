#pragma once
namespace bg {

enum RelocationType
{
    RelocationType_ABS,
    RelocationType_REL32,
    RelocationType_ADDR32,
    RelocationType_ADDR32NB,
    RelocationType_ADDR64,
};

struct Relocation
{
public:
    Relocation();

    Section *section;
    uint32_t rva;
    uint32_t symbol_index;
    RelocationType type;
};

} // namespace bg
