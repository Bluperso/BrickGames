#include <gtest/gtest.h>

#include "../snake_types.h"

namespace my_nsp {
namespace {

TEST(SnakeTypesTest, ConstantsMatchSpec) {
  EXPECT_EQ(kWidth, 10);
  EXPECT_EQ(kHeight, 20);
  EXPECT_EQ(kMaxLength, 200);
  EXPECT_EQ(kStartLength, 4);
  EXPECT_EQ(kMaxLevel, 10);
  EXPECT_EQ(kPointsPerLevel, 5);
}

TEST(SnakeTypesTest, OppositeAll) {
  EXPECT_EQ(Opposite(Direction::kUp), Direction::kDown);
  EXPECT_EQ(Opposite(Direction::kDown), Direction::kUp);
  EXPECT_EQ(Opposite(Direction::kLeft), Direction::kRight);
  EXPECT_EQ(Opposite(Direction::kRight), Direction::kLeft);
}

TEST(SnakeTypesTest, OppositeIsInvolution) {
  EXPECT_EQ(Opposite(Opposite(Direction::kUp)), Direction::kUp);
  EXPECT_EQ(Opposite(Opposite(Direction::kDown)), Direction::kDown);
  EXPECT_EQ(Opposite(Opposite(Direction::kLeft)), Direction::kLeft);
  EXPECT_EQ(Opposite(Opposite(Direction::kRight)), Direction::kRight);
}

TEST(SnakeTypesTest, LeftOfAll) {
  EXPECT_EQ(LeftOf(Direction::kUp), Direction::kLeft);
  EXPECT_EQ(LeftOf(Direction::kDown), Direction::kRight);
  EXPECT_EQ(LeftOf(Direction::kLeft), Direction::kDown);
  EXPECT_EQ(LeftOf(Direction::kRight), Direction::kUp);
}

TEST(SnakeTypesTest, RightOfAll) {
  EXPECT_EQ(RightOf(Direction::kUp), Direction::kRight);
  EXPECT_EQ(RightOf(Direction::kDown), Direction::kLeft);
  EXPECT_EQ(RightOf(Direction::kLeft), Direction::kUp);
  EXPECT_EQ(RightOf(Direction::kRight), Direction::kDown);
}

TEST(SnakeTypesTest, LeftRightAreInverses) {
  EXPECT_EQ(LeftOf(RightOf(Direction::kUp)), Direction::kUp);
  EXPECT_EQ(RightOf(LeftOf(Direction::kRight)), Direction::kRight);
  EXPECT_EQ(LeftOf(RightOf(Direction::kDown)), Direction::kDown);
  EXPECT_EQ(RightOf(LeftOf(Direction::kLeft)), Direction::kLeft);
}

TEST(SnakeTypesTest, LeftOfIsNotRightOf) {
  EXPECT_NE(LeftOf(Direction::kUp), RightOf(Direction::kUp));
}

TEST(SnakeTypesTest, PointDefaultIsZero) {
  Point p;
  EXPECT_EQ(p.x, 0);
  EXPECT_EQ(p.y, 0);
}

TEST(SnakeTypesTest, SnakeDefaultState) {
  Snake s;
  EXPECT_TRUE(s.body.empty());
  EXPECT_EQ(s.direction, Direction::kRight);
  EXPECT_EQ(s.next_direction, Direction::kRight);
}

TEST(SnakeTypesTest, AppleDefaultIsNoApple) {
  Apple a;
  EXPECT_EQ(a.pos.x, -1);
  EXPECT_EQ(a.pos.y, -1);
}

}  // namespace
}  // namespace my_nsp