#include <gtest/gtest.h>

#include "../snake_fsm.h"

namespace my_nsp {
namespace {

TEST(SnakeFSMTest, InitialStateIsStart) {
  SnakeFSM fsm;
  EXPECT_EQ(fsm.GetState(), SnakeState::kStart);
}

TEST(SnakeFSMTest, ResetReturnsToStart) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Reset();
  EXPECT_EQ(fsm.GetState(), SnakeState::kStart);
}

TEST(SnakeFSMTest, StartFromStartGoesToSpawn) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kSpawn);
}

TEST(SnakeFSMTest, TickFromSpawnGoesToMoving) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
}

TEST(SnakeFSMTest, PauseTogglesInMoving) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kPauseE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kPause);
  fsm.Fire(Event::kPauseE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
}

TEST(SnakeFSMTest, DirectionAndActionDoNotChangeState) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kLeftE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
  fsm.Fire(Event::kRightE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
  fsm.Fire(Event::kActionOnE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
  fsm.Fire(Event::kActionOffE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kMoving);
}

TEST(SnakeFSMTest, CollisionGoesToGameOver) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kCollisionE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kGameOver);
}

TEST(SnakeFSMTest, MaxLengthGoesToWin) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kMaxLengthE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kWin);
}

TEST(SnakeFSMTest, RestartFromGameOver) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kCollisionE);
  fsm.Fire(Event::kStartE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kSpawn);
}

TEST(SnakeFSMTest, RestartFromWin) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kMaxLengthE);
  fsm.Fire(Event::kStartE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kSpawn);
}

TEST(SnakeFSMTest, TerminateFromMoving) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kTerminateE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kTerminate);
}

TEST(SnakeFSMTest, TerminateFromPause) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kPauseE);
  fsm.Fire(Event::kTerminateE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kTerminate);
}

TEST(SnakeFSMTest, TerminateIsFinal) {
  SnakeFSM fsm;
  fsm.Fire(Event::kTerminateE);
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kTerminate);
}

TEST(SnakeFSMTest, PauseFromStartIsIgnored) {
  SnakeFSM fsm;
  fsm.Fire(Event::kPauseE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kStart);
}

TEST(SnakeFSMTest, CollisionFromPauseIsIgnored) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kTickE);
  fsm.Fire(Event::kPauseE);
  fsm.Fire(Event::kCollisionE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kPause);
}

TEST(SnakeFSMTest, LeftFromSpawnIsIgnored) {
  SnakeFSM fsm;
  fsm.Fire(Event::kStartE);
  fsm.Fire(Event::kLeftE);
  EXPECT_EQ(fsm.GetState(), SnakeState::kSpawn);
}

}  // namespace
}  // namespace my_nsp