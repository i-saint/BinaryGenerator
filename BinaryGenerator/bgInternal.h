#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace bg {

class Context;
class Section;
class SymbolTable;
class StringTable;

class Arch_x86;
class Arch_x64;
class Arch_ARM;

} // namespace bg

#include "Foundation/Foundation.h"
#include "../BinaryGenerator.h"
#include "bgString.h"
#include "bgSymbol.h"
#include "bgSection.h"
#include "bgContext.h"
using namespace fdn;
