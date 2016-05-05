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

size_t GetFileSize(const char *path)
{
    size_t ret = 0;
    if (FILE *f = fopen(path, "rb")) {
        fseek(f, 0, SEEK_END);
        ret = ftell(f);
        fclose(f);
    }
    return ret;
}

size_t ReadFile(const char *path, void *dst, size_t dst_size)
{
    size_t ret = 0;
    if (FILE *f = fopen(path, "rb")) {
        ret = fread(dst, 1, dst_size, f);
        fclose(f);
    }
    return ret;
}

} // namespace fdn
