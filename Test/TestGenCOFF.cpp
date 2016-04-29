#include "../BinaryGenerator.h"

void Generate_SayHello_COFF_x86()
{
    bg::IContext *ctx = bg::CreateContext();

    // void SayHello() { MessageBoxA("hello!"); }
    const char code[] = "\x55\x8B\xEC\x83\xEC\x34\x53\x56\x57\xC7\x45\xD8\x75\x73\x65\x72\xC7\x45\xDC\x33\x32\x2E\x64\xC7\x45\xE0\x6C\x6C\x00\x00\xC7\x45\xCC\x4D\x65\x73\x73\xC7\x45\xD0\x61\x67\x65\x42\xC7\x45\xD4\x6F\x78\x41\x00\xC7\x45\xE4\x68\x65\x6C\x6C\xC7\x45\xE8\x6F\x21\x00\x00\x33\xC9\x64\x8B\x35\x30\x00\x00\x00\x8B\x76\x0C\x8B\x76\x1C\x8B\x46\x08\x8B\x7E\x20\x8B\x36\x38\x4F\x18\x75\xF3\x89\x45\xF8\x8B\x55\xF8\x8B\x42\x3C\x8B\x44\x10\x78\x83\x65\xFC\x00\x03\xC2\x8B\x70\x20\x8B\x78\x24\x8B\x58\x1C\x8B\x40\x14\x03\xF2\x03\xFA\x03\xDA\x89\x45\xF4\x85\xC0\x74\x36\x8B\x45\xFC\x8B\x0C\x86\x0F\xB7\x04\x47\x8B\x04\x83\x03\xC2\x81\x7C\x11\x08\x61\x72\x79\x41\x75\x05\x89\x45\xF0\xEB\x0D\x81\x7C\x11\x04\x72\x6F\x63\x41\x75\x03\x89\x45\xEC\xFF\x45\xFC\x8B\x45\xFC\x3B\x45\xF4\x72\xCA\x8D\x45\xD8\x50\xFF\x55\xF0\x8D\x4D\xCC\x51\x50\xFF\x55\xEC\x6A\x00\x8D\x4D\xE4\x51\x51\x6A\x00\xFF\xD0\x5F\x5E\x5B\xC9\xC3";
    bg::ISection *text = ctx->createSection(".text", bg::SectionType_Text);
    text->addExternalSymbol(code, sizeof(code), "_SayHello");

    ctx->write("SayHello_x86.obj", bg::Format_COFF_x86);
    ctx->release();
}

void Generate_PutMessage_COFF_x64()
{
    bg::IContext *ctx = bg::CreateContext();

    // void PutMessage() { puts("hello!"); }
    const char string[] = "hello!";
    const char code[] = "\x48\x8D\x0D\x00\x00\x00\x00\xE9\x00\x00\x00\x00";

    bg::ISection *text = ctx->createSection(".text", bg::SectionType_Text);
    bg::Symbol sym_str  = text->addStaticSymbol(string, sizeof(string), "$hello");
    bg::Symbol sym_code = text->addExternalSymbol(code, sizeof(code), "PutMessage");
    text->addRelocation(sym_code.addr + 3, "$hello", bg::RelocationType_REL32);
    text->addRelocation(sym_code.addr + 8, "puts", bg::RelocationType_REL32);

    // option: dllexport PutMessage()
    bg::ISection *directive = ctx->createSection(".drectve", bg::SectionType_Info);
    const char exports[] = "/EXPORT:PutMessage ";
    directive->addStaticSymbol(exports, sizeof(exports), ".drectve");

    ctx->write("PutMessage_x64.obj", bg::Format_COFF_x64);
    ctx->release();
}


int main(int argc, char *argv[])
{
    Generate_SayHello_COFF_x86();
    Generate_PutMessage_COFF_x64();
}
