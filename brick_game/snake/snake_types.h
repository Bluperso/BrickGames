#pragma once

#include <deque>

namespace my_nsp {

static constexpr int kWidth = 10;
static constexpr int kPointsPerLevel = 5;
static constexpr int kHeight = 20;
static constexpr int kMaxLength = 200;
static constexpr int kStartLength = 4;
static constexpr int kMaxLevel = 10;

enum class Direction { kUp, kDown, kLeft, kRight };

struct Point {
  int x = 0;
  int y = 0;
  bool operator==(const Point &other) const {
    return ((x == other.x) && (y == other.y));
  }
};

// Змейка
struct Snake {
  std::deque<Point> body;
  Direction direction = Direction::kRight;
  Direction next_direction = Direction::kRight;
};

struct Apple {
  Point pos{-1, -1};
};

constexpr Direction Opposite(Direction d) {
  switch (d) {
    case Direction::kUp:
      return Direction::kDown;
    case Direction::kDown:
      return Direction::kUp;
    case Direction::kLeft:
      return Direction::kRight;
    case Direction::kRight:
      return Direction::kLeft;
  }
  return d;
}

constexpr Direction LeftOf(Direction d) {
  switch (d) {
    case Direction::kUp:
      return Direction::kLeft;
    case Direction::kDown:
      return Direction::kRight;
    case Direction::kLeft:
      return Direction::kDown;
    case Direction::kRight:
      return Direction::kUp;
  }
  return d;
}

constexpr Direction RightOf(Direction d) {
  switch (d) {
    case Direction::kUp:
      return Direction::kRight;
    case Direction::kDown:
      return Direction::kLeft;
    case Direction::kLeft:
      return Direction::kUp;
    case Direction::kRight:
      return Direction::kDown;
  }
  return d;
}
}  // namespace my_nsp