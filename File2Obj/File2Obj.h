#pragma once

#include <string>
#include <vector>
#include "../BinaryGenerator.h"

namespace f2o {

bool File2Obj(
    const char *path_to_input,
    const char *path_to_obj,
    const char *symbol_name,
    bg::Architecture arch,
    bg::Format fmt);

} // namespace dd
