#pragma once

template <class T>
struct Point {
  T x {0}, y {0};

  Point() = default;
  Point(auto nx, auto ny) noexcept: x(nx), y(ny) {}
  Point(auto other) noexcept: x(other.x), y(other.y) {}
  Point(auto &&other) noexcept: x(std::move(other.x)), y(std::move(other.y)) {}

  Point& operator = (auto other) noexcept {
    x = other.x;
    y = other.y;
    return *this;
  }

  Point& operator = (auto &&other) noexcept {
    x = std::move(other.x);
    y = std::move(other.y);
    return *this;
  }

  bool operator ==(auto other) const noexcept
    { return x == other.x && y == other.y; }
  
  bool operator !=(auto other) const noexcept
    { return x != other.x || y != other.y; }
}; // Point
