#include <windows.h>

typedef void(*SayHelloT)();
extern "C" void SayHello();

int main(int argc, char *argv[])
{
    SayHello();

#ifdef _M_IX86
    auto mod = LoadLibraryA("SayHello_x86.dll");
#elif _M_AMD64
    auto mod = LoadLibraryA("SayHello_x64.dll");
#endif
    auto* f = (SayHelloT)GetProcAddress(mod, "SayHello");
    f();
}
