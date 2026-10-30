#include <gtest/gtest.h>

#include "../snake_controller.h"

namespace my_nsp {
namespace {

TEST(SnakeControllerTest, InitialStateAfterConstruction) {
  SnakeController controller;
  GameInfo_t info = controller.updateCurrentState();
  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.level, 1);
}

TEST(SnakeControllerTest, PauseDoesNotCrash) {
  SnakeController controller;
  controller.userInput(Start, false);
  controller.userInput(Pause, false);
  EXPECT_NO_THROW(controller.updateCurrentState());
}

TEST(SnakeControllerTest, TerminateDelegatesToModel) {
  SnakeController controller;
  controller.userInput(Terminate, false);
  GameInfo_t info = controller.updateCurrentState();
  EXPECT_NE(info.field, nullptr);
}

TEST(SnakeControllerTest, LeftRightDoNotCrash) {
  SnakeController controller;
  controller.userInput(Start, false);
  controller.userInput(Left, false);
  controller.userInput(Right, false);
  EXPECT_NO_THROW(controller.updateCurrentState());
}

TEST(SnakeControllerTest, UpDownIgnored) {
  SnakeController controller;
  controller.userInput(Start, false);
  controller.userInput(Up, false);
  controller.userInput(Down, false);
  EXPECT_NO_THROW(controller.updateCurrentState());
}

TEST(SnakeControllerTest, ActionOnOffDoesNotCrash) {
  SnakeController controller;
  controller.userInput(Start, false);
  controller.userInput(Action, true);
  controller.userInput(Action, false);
  EXPECT_NO_THROW(controller.updateCurrentState());
}

}  // namespace
}  // namespace my_nsp