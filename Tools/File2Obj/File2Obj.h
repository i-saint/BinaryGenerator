#pragma once

#include <string>
#include <vector>

namespace f2o {

bool File2Obj(
    const char *in_path,
    const char *out_path,
    const char *symbol_name,
    const char *section_name,
    bg::SectionFlag flags,
    bg::Architecture arch,
    bg::Format fmt);

} // namespace dd
