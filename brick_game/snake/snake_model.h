#pragma once

#include <array>
#include <chrono>
#include <random>

#include "../common/types.h"
#include "high_score_rep.h"
#include "snake_fsm.h"
#include "snake_types.h"

namespace my_nsp {

class SnakeModel {
 public:
  explicit SnakeModel(
      std::chrono::milliseconds tick_interval = std::chrono::milliseconds{500},
      std::string high_score_path = DefaultHighScorePath());

  void Start();
  void TogglePause();
  void Terminate();
  void TurnLeft();
  void TurnRight();
  void SetDirection(Direction d);
  GameInfo_t UpdateCurrentState();
  void Tick();
  void SetBoost(bool hold);

  SnakeState GetState() const { return fsm_.GetState(); }
  void SetAppleForTest(const Point &p);
  Point GetHeadForTest() const { return snake_.body.front(); }

 private:
  void TickIfTimeElapsed();
  void Reset();
  void OnTick();
  void ApplyDirection();
  void MoveSnake();
  bool CheckWallCollision(const Point &head) const;
  bool CheckSelfCollision(const Point &head) const;
  GameInfo_t BuildGameInfo();
  bool CheckAppleEaten(const Point &head) const;
  void EatApple();
  void SpawnApple();
  void UpdateLevelAndSpeed();

  std::array<std::array<int, kWidth>, kHeight> field_{};
  std::array<int *, kHeight> field_rows_{};
  Snake snake_;
  Apple apple_;

  int score_ = 0;
  int high_score_ = 0;
  int level_ = 1;

  bool pause_ = false;
  bool boost_ = false;

  HighScoreRepository high_score_repo_;
  SnakeFSM fsm_;
  std::chrono::milliseconds tick_interval_;
  std::chrono::steady_clock::time_point last_tick_;
  std::mt19937 rng_;
};

}  // namespace my_nsp