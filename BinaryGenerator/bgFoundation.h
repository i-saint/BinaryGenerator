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

enum SectionType;
enum SymbolType;
enum RelocationType;

class Context;
class Section;
class Symbol;
class SymbolTable;
class Relocation;
class RelocationTable;
class String;
class StringTable;

} // namespace bg
