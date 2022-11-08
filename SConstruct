#!/usr/bin/env python
from platform import architecture
env = Environment()

is_debug = bool(int(ARGUMENTS.get("debug", 0)))
use_fflog = bool(int(ARGUMENTS.get("use_fflog", 0)))
cxx = ARGUMENTS.get("cxx", "g++")

# определение архитектуры
arch = architecture()
arch_bit = arch[0] # разрядность системы
arch_exe_fmt = arch[1] # формат экзешника
is_x64 = bool(arch_bit == "64bit")
is_linux = bool(arch_exe_fmt == "ELF")

# current compilation info
bitness = "x64" if is_x64 else "x32"
print(f"system bitness: {bitness}")
print(f"system executable format: {arch_exe_fmt}")
print(f"debug mode: {is_debug}")
print(f"compiler: {cxx}")

cpp_flags = ["-std=c++20", "-pipe", "-fopenmp"]
defines = []
ld_flags = ["-fopenmp"]

sysdef = ["-DLINUX"] if is_linux else ["-DWINDOWS"]
defines.extend(sysdef)
if (is_debug):
  defines.extend(["-DDEBUG"])
  cpp_flags.extend(["-O0", "-g"])
else: # release
  if (not is_linux):
      ld_flags.extend(["-shared-libstdc++"])
  ld_flags.extend(["-shared-libgcc", "-flto"])
  defines.extend(["-DNDEBUG"])
  cpp_flags.extend(["-s"])
  # "-msse4.1"
  cxx_opts = ["-Ofast", "-march=x86-64"] if is_x64 else ["-m32", "-Ofast", "-march=pentium2"]
  cpp_flags.extend(cxx_opts)
  cpp_flags.extend(["-flto"])
cxx_arch = ["-m64"] if is_x64 else ["-m32"]
cpp_flags.extend(cxx_arch)
if (use_fflog):
  defines.extend(["-DFFLOG"])

script = ARGUMENTS.get("script", "src/SConscript")

SConscript(
  script,
  exports=["cxx", "cpp_flags", "env", "is_linux", "defines", "ld_flags"]
)
