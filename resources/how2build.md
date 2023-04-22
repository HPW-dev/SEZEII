### How to build SEZE II
+ Install [Python3](https://www.python.org/downloads/) and [Scons](https://scons.org/doc/production/HTML/scons-user/ch01s02.html).
+ To build under Linux, install FFmpeg and SDL2 dev-packages.
+ Download SEZE dev. release from github .

execute these commands in project root:
+ For building SEZE II: ```scons .```
+ For building free plugins (output in ./bin/plugins/): ```scons -Q script=src/plugins/SConscript```
+ For building TVsim2conf: ```scons -Q script=src/tool/tvsim2.conf/SConscript```
+ For debug & watch ffmpeg debug log:
```
scons -Q debug=1 -Q fflog=1
scons -Q debug=1 -Q script=src/plugins/SConscript
```
### How to build seze-gui
Install [pyinstaller](https://www.pyinstaller.org/) and execute:
```
mkdir buid_gui
cd buid_gui
pyinstaller --onefile -c ../src/seze-gui.py -i ../resources/SEZEII.ico
```
Release files located in ```dist/``` directory
