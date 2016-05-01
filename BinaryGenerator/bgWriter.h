#pragma once
namespace bg {

// Arch: Arch_x86, Arch_x64
template<class Arch> bool PECOFFWriteObj(Context& ctx, IOutputStream& os);
template<class Arch> bool PECOFFWriteExe(Context& ctx, IOutputStream& os);
template<class Arch> bool PECOFFWriteDLL(Context& ctx, IOutputStream& os);

template<class Arch> bool ELFWriteObj(Context& ctx, IOutputStream& os);
template<class Arch> bool ELFWriteExe(Context& ctx, IOutputStream& os);
template<class Arch> bool ELFWriteDLL(Context& ctx, IOutputStream& os);

} // namespace bg
