
#include <gtest/gtest.h>

#include "../../common/game_factory.h"

namespace my_nsp {
namespace {

TEST(GameFactoryTest, CreateSnakeReturnsNonNull) {
  auto game = CreateGame(GameType::kSnake);
  ASSERT_NE(game, nullptr);
  GameInfo_t info = game->updateCurrentState();
  EXPECT_NE(info.field, nullptr);
}

TEST(GameFactoryTest, CreateTetrisReturnsNonNull) {
  auto game = CreateGame(GameType::kTetris);
  ASSERT_NE(game, nullptr);
  GameInfo_t info = game->updateCurrentState();
  EXPECT_NE(info.field, nullptr);
}

TEST(GameFactoryTest, SnakeNextFigureTypeIsZero) {
  auto game = CreateGame(GameType::kSnake);
  EXPECT_EQ(game->GetNextFigureType(), 0);
}

TEST(GameFactoryTest, TetrisNextFigureTypeInRange) {
  auto game = CreateGame(GameType::kTetris);
  int type = game->GetNextFigureType();
  EXPECT_GE(type, 1);
  EXPECT_LE(type, 7);
}

TEST(GameFactoryTest, SnakeStartAndUpdate) {
  auto game = CreateGame(GameType::kSnake);
  game->userInput(Start, false);
  GameInfo_t info = game->updateCurrentState();
  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.level, 1);
}

TEST(GameFactoryTest, TetrisStartAndUpdate) {
  auto game = CreateGame(GameType::kTetris);
  game->userInput(Start, false);
  game->updateCurrentState();
  EXPECT_EQ(game->GetStatus(), GameStatus::kRunning);
}

TEST(GameFactoryTest, TetrisPauseStatus) {
  auto game = CreateGame(GameType::kTetris);
  game->userInput(Start, false);
  game->updateCurrentState();
  game->userInput(Pause, false);
  EXPECT_EQ(game->GetStatus(), GameStatus::kPaused);
}

}  // namespace
}  // namespace my_nsp