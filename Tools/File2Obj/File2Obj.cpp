#include "pch.h"
#include "Foundation/Foundation.h"
#include "BinaryGenerator/BinaryGenerator.h"
#include "File2Obj.h"

namespace f2o {

bool File2Obj(
    const char *in_path,
    const char *out_path,
    const char *sym_name,
    const char *section_name,
    bg::SectionFlag flags,
    bg::Architecture arch,
    bg::Format fmt)
{
    uint32_t size = (uint32_t)fdn::GetFileSize(in_path);
    std::string size_name = sym_name;
    size_name += "_size";


    std::string decolated_sym_name, decolated_size_name;
    if (arch == bg::Architecture::x86) {
        decolated_sym_name = std::string("_") + sym_name;
        decolated_size_name = std::string("_") + size_name;
    }
    else {
        decolated_sym_name = sym_name;
        decolated_size_name = size_name;
    }


    bool result = false;
    if (fmt == bg::Format::PECOFF) {
        auto *ctx = bg::CreatePECOFFContext(arch);
        auto *idata = ctx->createSection(section_name, flags);
        auto sym = idata->addExternalSymbol(nullptr, size, decolated_sym_name.c_str());
        fdn::ReadFile(in_path, idata->getData(sym), size);
        idata->addExternalSymbol(&size, sizeof(size), decolated_size_name.c_str());
        result = ctx->writeObj(out_path);
        ctx->release();
    }
    else if (fmt == bg::Format::ELF) {
        printf("ELF is not supported yet.\n");
        return false;
    }

    if (!result) {
        printf("generated %s\n", out_path);
    }

    if (result) {
        printf("generated %s\n", out_path);

        std::string path_h;
        auto *beg = out_path;
        auto *end = fdn::GetFileExt(fdn::GetFileName(out_path));
        if (beg == end) {
            path_h.assign(beg);
        }
        else {
            path_h.assign(beg, end - 1);
        }
        path_h += ".h";

        auto ofs = std::ofstream(path_h.c_str(), std::ios::out | std::ios::binary);
        if (ofs) {
            char buf[2048];
            sprintf(buf,
                "extern \"C\" {\n"
                "    extern const char %s[0x%x];\n"
                "    extern const unsigned int %s;\n"
                "}", sym_name, size, size_name.c_str());
            ofs << buf;

            printf("generated %s\n", path_h.c_str());
        }
    }
    return result;
}

} // namespace dd

int main(int argc, char *argv[])
{
    std::string in_file;
    std::string out_file;
    std::string symbol_name;
    std::string section_name = ".rdata";
    bg::Architecture arch = bg::Architecture::x64;
    bg::Format fmt = bg::Format::PECOFF;
    bg::SectionFlag flags = bg::SectionFlag::IDataSection;

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
        else if (strncmp(arg, "/section:", 9) == 0) {
            arg += 9;
            section_name = arg;
        }
        else if (strncmp(arg, "/flags:", 7) == 0) {
            arg += 7;
            flags = bg::SectionFlag::IData;
            while (*arg != 0) {
                if (*arg == 'R') { flags = flags | bg::SectionFlag::Read; }
                if (*arg == 'W') { flags = flags | bg::SectionFlag::Write; }
                if (*arg == 'X') { flags = flags | bg::SectionFlag::Execute; }
                ++arg;
            }
        }
        else if (strncmp(arg, "/arch:", 6) == 0) {
            arg += 6;
            if (strcmp(arg, "x86") == 0) {
                arch = bg::Architecture::x86;
            }
            else if (strcmp(arg, "x86_64") == 0 || strcmp(arg, "x64") == 0) {
                arch = bg::Architecture::x64;
            }
        }
        else if (strncmp(arg, "/format:", 8) == 0) {
            arg += 8;
            if (strcmp(arg, "PE") == 0 || strcmp(arg, "COFF") == 0 || strcmp(arg, "PECOFF") == 0) {
                fmt = bg::Format::PECOFF;
            }
            else if (strcmp(arg, "ELF") == 0) {
                fmt = bg::Format::ELF;
            }
        }
    }

    if (symbol_name.empty() && !out_file.empty()) {
        auto *beg = fdn::GetFileName(out_file.c_str());
        auto *end = fdn::GetFileExt(beg);
        if (beg == end) {
            symbol_name.assign(beg);
        }
        else {
            symbol_name.assign(beg, end - 1);
        }
    }

    if (in_file.empty() || out_file.empty() || symbol_name.empty()) {
        printf(
            "usage: file2obj /in:input_file /out:output_file [options]\n"
            "options:\n"
            "  /symbol:symbol_name (default: filename of output_file)\n"
            "  /section:section_name (default: .rdata)\n"
            "  /flags:[RWX] (default: R)\n"
            "  /arch:(x86|x86_64) (default: x86_64)\n"
            "  /format:(PECOFF|ELF) (default: PECOFF)\n");
        return 0;
    }
    if (!fdn::FileExists(in_file.c_str())) {
        printf("inpt file %s doesn't exist\n", in_file.c_str());
        return 1;
    }

    return f2o::File2Obj(
        in_file.c_str(), out_file.c_str(), symbol_name.c_str(), section_name.c_str(),
        flags, arch, fmt) ? 0 : 1;
}
