#pragma once
namespace fdn {

const char* GetFileName(const char *path);
const char* GetFileExt(const char *path);
bool FileExists(const char *path);
size_t GetFileSize(const char *path);
size_t ReadFile(const char *path, void *dst, size_t dst_size);

// F: [](size_t size) -> void* : alloc func (e.g.: malloc)
template<class F>
inline bool MapFile(const char *path, void *&o_data, size_t &o_size, const F &alloc)
{
    o_data = nullptr;
    o_size = 0;

    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary);
    ifs.seekg(0, std::ios::end);
    o_size = (size_t)ifs.tellg();
    if (o_size > 0) {
        o_data = alloc(o_size);
        ifs.seekg(0, std::ios::beg);
        ifs.read((char*)o_data, o_size);
        return true;
    }
    return false;
}

} // namespace fdn
