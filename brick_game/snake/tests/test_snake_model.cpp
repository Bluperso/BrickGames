#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <thread>

#include "../snake_model.h"

namespace my_nsp {
namespace {

TEST(SnakeModelTest, InitialStateIsStart) {
  SnakeModel model;
  EXPECT_EQ(model.GetState(), SnakeState::kStart);
}

TEST(SnakeModelTest, StartMovesToSpawn) {
  SnakeModel model;
  model.Start();
  EXPECT_EQ(model.GetState(), SnakeState::kSpawn);
}

TEST(SnakeModelTest, FirstTickMovesToMoving) {
  SnakeModel model;
  model.Start();
  model.Tick();
  EXPECT_EQ(model.GetState(), SnakeState::kMoving);
}

TEST(SnakeModelTest, UpdateReturnsValidField) {
  SnakeModel model;
  model.Start();
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  ASSERT_NE(info.field, nullptr);

  int snake_cells = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (info.field[y][x] == 1) ++snake_cells;
  EXPECT_EQ(snake_cells, kStartLength);

  int apples = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (info.field[y][x] == 2) ++apples;
  EXPECT_EQ(apples, 1);
}

TEST(SnakeModelTest, InitialScoreAndLevel) {
  SnakeModel model;
  model.Start();
  GameInfo_t info = model.UpdateCurrentState();
  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.level, 1);
  EXPECT_FALSE(info.pause);
}

TEST(SnakeModelTest, TerminateFromStart) {
  SnakeModel model;
  model.Terminate();
  EXPECT_EQ(model.GetState(), SnakeState::kTerminate);
}

TEST(SnakeModelTest, SnakeLengthStaysConstantWithoutApple) {
  SnakeModel model;
  model.Start();
  model.Tick();
  for (int i = 0; i < 3; ++i) {
    model.Tick();
  }

  GameInfo_t info = model.UpdateCurrentState();
  int snake_cells = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (info.field[y][x] == 1) ++snake_cells;

  EXPECT_EQ(snake_cells, kStartLength);
}

TEST(SnakeModelTest, WallCollisionGoesToGameOver) {
  SnakeModel model;
  model.Start();
  model.Tick();
  for (int i = 0; i < 20 && model.GetState() != SnakeState::kGameOver; ++i) {
    model.Tick();
  }

  EXPECT_EQ(model.GetState(), SnakeState::kGameOver);
}

TEST(SnakeModelTest, GameOverStopsMovement) {
  SnakeModel model;
  model.Start();
  model.Tick();

  for (int i = 0; i < 20 && model.GetState() != SnakeState::kGameOver; ++i) {
    model.Tick();
  }
  ASSERT_EQ(model.GetState(), SnakeState::kGameOver);

  GameInfo_t before = model.UpdateCurrentState();
  std::array<std::array<int, kWidth>, kHeight> snapshot{};
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x) snapshot[y][x] = before.field[y][x];
  model.Tick();
  GameInfo_t after = model.UpdateCurrentState();

  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      EXPECT_EQ(after.field[y][x], snapshot[y][x]);
}

TEST(SnakeModelTest, TimerTriggersTickInUpdateCurrentState) {
  SnakeModel model{std::chrono::milliseconds{1}};
  model.Start();
  model.Tick();
  GameInfo_t before = model.UpdateCurrentState();
  int before_head_x = -1;
  int before_head_y = -1;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (before.field[y][x] == 1) {
        if (x > before_head_x) {
          before_head_x = x;
          before_head_y = y;
        }
      }

  std::this_thread::sleep_for(std::chrono::milliseconds{5});
  GameInfo_t after = model.UpdateCurrentState();

  int after_head_x = -1;
  int after_head_y = -1;
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      if (after.field[y][x] == 1 && x > after_head_x) {
        after_head_x = x;
        after_head_y = y;
      }
    }
  }
  EXPECT_GT(after_head_x, before_head_x);
  EXPECT_EQ(after_head_y, before_head_y);
}
TEST(SnakeModelTest, EatingAppleIncreasesScore) {
  SnakeModel model;
  model.Start();
  model.Tick();
  model.SetAppleForTest({6, 10});
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  EXPECT_EQ(info.score, 1);
}

TEST(SnakeModelTest, EatingAppleIncreasesLength) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetAppleForTest({6, 10});
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  int cells = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (info.field[y][x] == 1) ++cells;

  EXPECT_EQ(cells, kStartLength + 1);
}

TEST(SnakeModelTest, AppleRespawnsAfterEating) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetAppleForTest({6, 10});
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  int apples = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (info.field[y][x] == 2) ++apples;

  EXPECT_EQ(apples, 1);
}

TEST(SnakeModelTest, AppleDoesNotSpawnOnSnake) {
  SnakeModel model;
  model.Start();
  model.Tick();
  for (int i = 0; i < 10 && model.GetState() == SnakeState::kMoving; ++i) {
    Point head = model.GetHeadForTest();
    Point next{head.x + 1, head.y};
    if (next.x >= kWidth) break;
    model.SetAppleForTest(next);
    model.Tick();

    GameInfo_t info = model.UpdateCurrentState();
    for (int y = 0; y < kHeight; ++y) {
      for (int x = 0; x < kWidth; ++x) {
        if (info.field[y][x] == 2) {
          EXPECT_NE(info.field[y][x], 1);
        }
      }
    }
  }
}

TEST(SnakeModelTest, LevelStaysOneBeforeFivePoints) {
  SnakeModel model;
  model.Start();
  model.Tick();
  model.SetAppleForTest({6, 10});
  model.Tick();
  model.SetAppleForTest({7, 10});
  model.Tick();
  model.SetAppleForTest({8, 10});
  model.Tick();
  model.SetAppleForTest({9, 10});
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  EXPECT_EQ(info.score, 4);
  EXPECT_EQ(info.level, 1);
}

TEST(SnakeModelTest, ScoreDoesNotIncreaseWithoutApple) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetAppleForTest({0, 0});
  model.Tick();
  model.Tick();

  GameInfo_t info = model.UpdateCurrentState();
  EXPECT_EQ(info.score, 0);
}

TEST(SnakeModelTest, TurnRightFromRightGoesDown) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.TurnRight();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 11);
}

TEST(SnakeModelTest, TurnLeftFromRightGoesUp) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.TurnLeft();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 9);
}

TEST(SnakeModelTest, TurnDoesNotMoveImmediately) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.TurnRight();
  Point head = model.GetHeadForTest();
  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, DoubleTurnLeftDoesNotReverse) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.TurnLeft();
  model.TurnLeft();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 9);
}

TEST(SnakeModelTest, DoubleTurnRightDoesNotReverse) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.TurnRight();
  model.TurnRight();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 11);
}

TEST(SnakeModelTest, TurnIgnoredInSpawn) {
  SnakeModel model;
  model.Start();
  model.TurnRight();
  model.Tick();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, TurnIgnoredInPause) {
  SnakeModel model;
  model.Start();
  model.Tick();
  model.TogglePause();

  model.TurnRight();
  model.TogglePause();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, TurnIgnoredInGameOver) {
  SnakeModel model;
  model.Start();
  model.Tick();

  for (int i = 0; i < 20 && model.GetState() != SnakeState::kGameOver; ++i) {
    model.Tick();
  }
  ASSERT_EQ(model.GetState(), SnakeState::kGameOver);

  model.TurnLeft();
  model.TurnRight();
  EXPECT_EQ(model.GetState(), SnakeState::kGameOver);
}

TEST(SnakeModelTest, SetDirectionUpFromRight) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetDirection(Direction::kUp);
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 9);
}

TEST(SnakeModelTest, SetDirectionDownFromRight) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetDirection(Direction::kDown);
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 11);
}

TEST(SnakeModelTest, SetDirectionBackwardsIsIgnored) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetDirection(Direction::kLeft);
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, SetDirectionSameIsIgnored) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetDirection(Direction::kRight);
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, SetDirectionIgnoredInSpawn) {
  SnakeModel model;
  model.Start();

  model.SetDirection(Direction::kUp);
  model.Tick();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, SetDirectionIgnoredInPause) {
  SnakeModel model;
  model.Start();
  model.Tick();
  model.TogglePause();
  model.SetDirection(Direction::kUp);
  model.TogglePause();
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 6);
  EXPECT_EQ(head.y, 10);
}

TEST(SnakeModelTest, SetDirectionThenReverseBeforeTick) {
  SnakeModel model;
  model.Start();
  model.Tick();

  model.SetDirection(Direction::kUp);
  model.SetDirection(Direction::kDown);
  model.Tick();
  Point head = model.GetHeadForTest();

  EXPECT_EQ(head.x, 5);
  EXPECT_EQ(head.y, 11);
}

}  // namespace
}  // namespace my_nsp