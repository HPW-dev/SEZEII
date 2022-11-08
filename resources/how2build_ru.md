### Как скомпилировать SEZE II
+ Установите [Python3](https://www.python.org/downloads/) и [Scons](https://scons.org/doc/production/HTML/scons-user/ch01s02.html).
+ Для Линукса установите dev-пакеты FFmpeg и SDL2.
+ Скачайте dev-релиз SEZE с github.

Выполните следующие команды в корневом каталоге SEZE II:
+ Для сборки SEZE II: ```scons .```
+ Для сборки плагинов (искать в папке ./bin/plugins/): ```scons -Q script=src/plugins/SConscript```
+ Для сборки TVsim2conf: ```scons -Q script=src/tool/tvsim2bw.conf/SConscript```
+ Для отладки и просмотра ffmpeg логов:
```
scons -Q debug=1 -Q fflog=1
scons -Q debug=1 -Q script=src/plugins/SConscript
```
### Как скомпилировать seze-gui
Установите [pyinstaller](https://www.pyinstaller.org/) и выполните команды:
```
mkdir buid_gui
cd buid_gui
pyinstaller --onefile -c ../src/seze-gui.py -i ../resources/SEZEII.ico
```
Файлы релиза вы найдёте в папке ```dist/```
