#include "pch.h"
#include "Foundation/Foundation.h"
#include "File2Obj.h"

namespace f2o {

bool File2Obj(
    const char *path_to_input,
    const char *path_to_obj,
    const char *symbol_name,
    bg::Architecture arch,
    bg::Format fmt)
{
    void *content = nullptr;
    size_t content_size = 0;
    fdn::MapFile(path_to_input, content, content_size, malloc);
    if (content == nullptr) { return false; }

    uint32_t size = (uint32_t)content_size;
    char size_name[1024];
    sprintf(size_name, "%s_size", symbol_name);

    bool result = false;
    if (fmt == bg::Format::PECOFF) {
        auto *ctx = bg::CreatePECOFFContext(arch);
        auto *idata = ctx->createSection(".idata", bg::SectionFlag::IDataSection);
        idata->addExternalSymbol(content, size, symbol_name);
        idata->addExternalSymbol(&size, sizeof(size), size_name);
        result = ctx->writeObj(path_to_obj);
        ctx->release();
    }
    else if (fmt == bg::Format::ELF) {

    }

    free(content);
    return result;
}

} // namespace dd

int main(int argc, char *argv[])
{
    const char *in_file = nullptr;
    const char *out_file = nullptr;
    const char *symbol_name = nullptr;
    bg::Architecture arch = bg::Architecture::x64;
    bg::Format fmt = bg::Format::PECOFF;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strncmp(arg, "/in:", 4) == 0) {
            arg += 4;
            in_file = arg;
        }
        else if (strncmp(arg, "/out:", 5) == 0) {
            arg += 5;
            out_file = arg;
        }
        else if (strncmp(arg, "/symbol:", 8) == 0) {
            arg += 8;
            symbol_name = arg;
        }
        else if (strncmp(arg, "/arch:", 6) == 0) {
            arg += 6;
            if (strcmp(arg, "x86") == 0) {
                arch = bg::Architecture::x86;
            }
            else if (strcmp(arg, "x86_64") == 0) {
                arch = bg::Architecture::x86;
            }
        }
        else if (strncmp(arg, "/format:", 8) == 0) {
            arg += 8;
            if (strcmp(arg, "PECOFF") == 0) {
                fmt = bg::Format::PECOFF;
            }
            else if (strcmp(arg, "ELF") == 0) {
                fmt = bg::Format::ELF;
            }
        }
    }

    if (!in_file || !out_file || !symbol_name) {
        printf("usage: file2obj /in:input_file /out:output_file /symbol:symbol_name /arch:(x86|x86_64) /format:(PECOFF|ELF)");
        return 1;
    }

    return f2o::File2Obj(in_file, out_file, symbol_name, arch, fmt) ? 0 : 1;
}
