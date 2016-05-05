#include "pch.h"
#include "Foundation.h"
namespace fdn {

const char* GetFileName(const char *path)
{
    auto *p = path;
    auto *ret = p;
    while (*p != '\0') {
        if (*p == '/' || *p == '\\') { ret = p + 1; }
        ++p;
    }
    return ret;
}

const char* GetFileExt(const char *path)
{
    path = GetFileName(path);
    auto *p = path;
    auto *ret = p;
    while (*p != '\0') {
        if (*p == '.') { ret = p + 1; }
        ++p;
    }
    return ret;
}

bool FileExists(const char *path)
{
    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary);
    return (bool)ifs;
}

size_t GetFileSize(const char *path)
{
    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary);
    return ifs ? (size_t)ifs.seekg(0, std::ios::end).tellg() : 0;
}

size_t ReadFile(const char *path, void *dst, size_t dst_size)
{
    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary);
    return ifs ? (size_t)ifs.read((char*)dst, dst_size).gcount() : 0;
}

} // namespace fdn
