#pragma once

#define bgCLinkage extern "C"
#ifdef _WIN32
    #ifndef bgStaticLink
        #ifdef bgImpl
            #define bgAPI __declspec(dllexport)
        #else
            #define bgAPI __declspec(dllimport)
        #endif
    #else
        #define bgAPI
    #endif
#else
    #define bgAPI
#endif

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>

namespace bg {

enum SectionFlag;
enum SymbolType;
enum RelocationType;

struct String;
struct Symbol;
struct Relocation;

class Context;
class Section;
class SymbolTable;
class StringTable;

class Traits_x86;
class Traits_x86_64;
class Traits_ARM;

} // namespace bg
