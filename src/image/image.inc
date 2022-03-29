/// @file seze::Image template funcs impl.

template<class T> T& fast_get(int i) const {
  auto p = rcast(T*, data_);
  return p[i];
}
template<class T> T& fast_get(int x, int y) const {
  auto p = rcast(T*, data_);
  return p[X * y + x];
}
template<class T> void fast_set(int i, CN(T) c) const {
  auto p = rcast(T*, data_);
  p[i] = c;
}
template<class T> void fast_set(int x, int y, CN(T) c) const {
  auto p = rcast(T*, data_);
  p[X * y + x] = c;
}
template<class T> T get(int x, int y,
boundig_e mode = boundig_e::none) const {
  if (prepare_cord(x, y, mode)) {
    auto p = rcast(T*, data_);
    return p[X * y + x];
  }
  return {};
} // get
template<class T> T* get_ptr(int x, int y,
boundig_e mode = boundig_e::none) const {
  if (prepare_cord(x, y, mode)) {
    auto p = rcast(T*, data_);
    return &p[X * y + x];
  }
  return nullptr;
}
template<class T> void set(int x, int y, CN(T) c, 
boundig_e mode = boundig_e::none) const {
  if ( !prepare_cord(x, y, mode))
    return;
  auto p = rcast(T*, data_);
  p[X * y + x] = c;
}
