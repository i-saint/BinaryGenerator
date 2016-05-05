#include "pch.h"
#include "Foundation.h"
namespace fdn {

const char* GetFilename(const char *path)
{
    size_t len = strlen(path);
    size_t separator = 0;
    for (size_t i = 0; i < len; ++i) {
        if (path[i] == '/' || path[i] == '\\') { separator = i + 1; }
    }
    return path + separator;
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
