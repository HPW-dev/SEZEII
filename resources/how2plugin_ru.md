# Как создать свой видео фильтр
[Есть видео на русском, как сделать эффект](https://youtu.be/tNvp_of47KU)

Скачайте [SEZE II plugin API](../plugin-api.h) и приступите к созданию плагина на C++.\
Посмотрите [код моих плагинов](../src/plugins) или воспользуйтесь этим примером инверсии цвета:
```c
// invert.c
#include "plugin-api.h"

// Инициализация плагина
struct PluginInfo init(const char* options) {
  struct PluginInfo info;
  PluginInfo_init(&info);
  info.color_type = seze_RGB24;
  info.title = "color inversion";
  return info;
}

/*
Обработка кадра
dst - RAW данные пикселей входного кадра
mx - ширина кадра
my - высота кадра
stride - размер строки кадра в байтах (mx * размер компонентов пикселя)
color_type - тип данных пикселя
*/
void core(byte* dst, int mx, int my, int stride, enum color_t color_type) {
  int bytes = my * stride;
  for (int i = 0; i < bytes; ++i)
    dst[i] = ~dst[i]; // инверсия каждого компонента пикселей
}

// Действия по завершению работы плагина
void finalize() {}
```
Для компиляции: ```g++ -shared invert.c -o invert.dll```\
Запустите ```seze``` или ```seze-gui``` и найдите файл своего плагина чтобы протестировать его работу
