# Как скомпилировать SEZE II
+ Установите [Python3](https://www.python.org/downloads/) и [Scons](https://scons.org/doc/production/HTML/scons-user/ch01s02.html).
+ Для линукса установти dev-пакеты FFmpeg и SDL2.
+ Скачайте dev-релиз SEZE с github.

Выполните следующие команды в корневом каталоге SEZE II:\
+ Для сборки SEZE II: ```scons .```
+ Для сборки свободных плагинов (output in ./bin/plugins/): ```scons . -Q build_plugins=1```
+ Для отладки и просмотра ffmpeg логов:
```
scons -Q debug=0 -Q build_plugins=0 -Q fflog=0
scons -Q debug=0 -Q build_plugins=1
```
### Как скомпилировать seze-gui
Установите [pyinstaller](https://www.pyinstaller.org/) и выполните команды:
```
mkdir buid_gui
cd buid_gui
pyinstaller --onefile -c ../src/seze-gui.py -i ../resources/SEZEII.ico
```
Файлы релиза вы найдёте в папке ```dist/```
