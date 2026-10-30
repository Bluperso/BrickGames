#include <gtest/gtest.h>

#include "../../tetris/tetris_controller.h"

namespace my_nsp {
namespace {

TEST(TetrisControllerTest, InitialStatusIsRunning) {
  TetrisController c;
  EXPECT_EQ(c.GetStatus(), GameStatus::kRunning);
}

TEST(TetrisControllerTest, NextFigureTypeInRange) {
  TetrisController c;
  int t = c.GetNextFigureType();
  EXPECT_GE(t, 1);
  EXPECT_LE(t, 7);
}

TEST(TetrisControllerTest, UserInputDoesNotCrash) {
  TetrisController c;
  EXPECT_NO_THROW(c.userInput(Start, false));
  EXPECT_NO_THROW(c.userInput(Left, false));
  EXPECT_NO_THROW(c.userInput(Right, false));
  EXPECT_NO_THROW(c.userInput(Up, false));
  EXPECT_NO_THROW(c.userInput(Down, false));
  EXPECT_NO_THROW(c.userInput(Pause, false));
  EXPECT_NO_THROW(c.updateCurrentState());
}

TEST(TetrisControllerTest, ActionStartsWhenInStart) {
  TetrisController c;
  c.userInput(Action, false);  // должно быть эквивалентно Start
  GameInfo_t info = c.updateCurrentState();
  EXPECT_NE(info.field, nullptr);
}

TEST(TetrisControllerTest, PauseToggles) {
  TetrisController c;
  c.userInput(Start, false);
  c.updateCurrentState();
  c.userInput(Pause, false);
  EXPECT_EQ(c.GetStatus(), GameStatus::kPaused);
  c.userInput(Pause, false);
  EXPECT_EQ(c.GetStatus(), GameStatus::kRunning);
}

TEST(TetrisControllerTest, TerminateSetsGameOver) {
  TetrisController c;
  c.userInput(Terminate, false);
  EXPECT_EQ(c.GetStatus(), GameStatus::kGameOver);
}

}  // namespace
}  // namespace my_nsp