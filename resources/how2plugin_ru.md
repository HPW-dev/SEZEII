# Как создать свой видео фильтр
Скачайте [SEZE II plugin API](../plugin-api.hpp) и приступите к созданию плагина на C++.\
Посмотрите [код моих плагинов](../src/plugins) или воспользуйтесь этим примером инверсии цвета:
```cpp
// invert.cpp
#include "plugin-api.hpp"
#include <algorithm>

// Для того чтобы функции именавались в C-стиле при компиляции в библиотеку
extern "C" {
  PluginInfo init(const std::string& options);
  void core(byte* dst, int x, int y, int stride, color_t color_type);
  void finalize();
}

// Инициализация плагина
PluginInfo init(const std::string& options) {
  PluginInfo info;
  info.color_type = color_t::RGB24;
  info.title = "Color inversion";
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
void core(byte* dst, int mx, int my, int stride, color_t color_type) {
  auto size_in_bytes = my * stride;
  // invert all pixel components
  std::transform(dst, dst + size_in_bytes, dst,
    [](byte pix)->byte { return ~pix; });
}

// Действия по завершению работы плагина
void finalize() {}
```
Для компиляции: ```g++ -shared invert.cpp```\
Запустите ```seze``` или ```seze-gui``` и найдите файл своего плагина чтобы протестировать его работу
