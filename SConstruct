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

cpp_flags = ["-std=c++23", "-pipe", "-fopenmp"]
defines = []
ld_flags = ["-fopenmp", "-shared-libgcc"]

sysdef = ["-DLINUX"] if is_linux else ["-DWINDOWS"]
defines.extend(sysdef)
if (is_debug):
  defines.extend(["-DDEBUG"])
  cpp_flags.extend(["-O0", "-g"])
else: # release
  ld_flags.extend(["-flto"])
  defines.extend(["-DNDEBUG"])
  cpp_flags.extend(["-s"])
  if is_x64:
    #cxx_opts = ["-Ofast", "-march=x86-64", "-mtune=generic"]
    cxx_opts = ["-O2"]
  else:
    cxx_opts = ["-Ofast", "-march=nocona", "-mtune=generic"]
  cpp_flags.extend(cxx_opts)
cxx_arch = ["-m64"] if is_x64 else ["-m32"]
cpp_flags.extend(cxx_arch)
if (use_fflog):
  defines.extend(["-DFFLOG"])

script = ARGUMENTS.get("script", "src/SConscript")

SConscript(
  script,
  exports=["cxx", "cpp_flags", "env", "is_linux", "defines", "ld_flags"]
)
