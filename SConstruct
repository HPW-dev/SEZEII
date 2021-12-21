#!/usr/bin/env python
from platform import architecture
env = Environment()

# consts
linux_exe_fmt = "ELF"
compiler_clangpp = "clang++"
compiler_gpp = "g++"
is_debug = bool(int(ARGUMENTS.get("debug", 0)))
build_plugins = bool(int(ARGUMENTS.get("build_plugins", 0)))
fflog = bool(int(ARGUMENTS.get("fflog", 0)))

# quess architecture
arch_raw = architecture()
arch_bit = arch_raw[0] # разрядность системы
arch_exe_fmt = arch_raw[1] # формат экзешника
bitness = {"x32": "32bit", "x64": "64bit"}

# current compilation info
print(f"system bitness: {arch_bit}")
print(f"system executable format: {arch_exe_fmt}")
print(f"debug mode: {is_debug}")

# var for build
compiler = ARGUMENTS.get("cxx", compiler_gpp)
cpp_flags = [
  "-std=c++20", "-pipe",
]
defines = [
  #"-DTVSIM_LD_COMPONENT",
]
ld_flags = []

# platform spec-s settings
is_linux = bool(arch_exe_fmt == linux_exe_fmt)
if (is_linux): # linux
  defines.append("-DLINUX")
  #compiler = compiler_clangpp
else: # windows
  defines.append("-DWINDOWS")

# exclude clibs
if ((compiler != compiler_clangpp) and (not is_debug)):
  ld_flags.extend(["-shared-libgcc", "-flto"])
  if (not is_linux):
    ld_flags.extend(["-shared-libstdc++"])

#bitness specific
if (arch_bit == bitness["x64"]):
  cpp_flags.extend(["-m64"])
else:
  cpp_flags.extend(["-m32"])
# debug/release settings:
if is_debug:
  defines.extend(["-DDEBUG", ])
  cpp_flags.extend(["-Og", "-ggdb3"])
else: # release
  defines.extend(["-DNDEBUG"])
  if (compiler != compiler_clangpp):
    cpp_flags.append("-s") # stripping exec
  #bitness specific
  if (arch_bit == bitness["x64"]):
    cpp_flags.extend(["-Ofast", "-march=x86-64"])
  else: # 32bit
    #cpp_flags.extend(["-O2"])
    #cpp_flags.extend(["-m32", "-Ofast", "-march=pentium2"])
    cpp_flags.extend(["-m32", "-Ofast", "-march=core2", "-msse4.1"])

# print selected compiler:
print(f"compiler: {compiler}")

# on/off ffmpeg debug logging
if (fflog):
  defines.append("-DFFLOG")

# билд эффектов:
if (build_plugins):
  print("building plugins enabled")
  SConscript("src/plugins/SConscript",
  exports=[
    "env", "is_linux", "compiler", "defines",
    "cpp_flags", "ld_flags"
  ])

# билд seze
if (not build_plugins):
  print("building seze enabled")
  SConscript("src/SConscript",
  exports=["compiler", "cpp_flags", "compiler_clangpp",
    "env", "is_linux", "defines", "ld_flags"])
