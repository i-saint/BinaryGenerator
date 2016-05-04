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

} // namespace fdn
