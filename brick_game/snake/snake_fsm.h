#pragma once

namespace my_nsp {

enum class SnakeState {
  kStart,
  kSpawn,
  kPause,
  kMoving,
  kGameOver,
  kWin,
  kTerminate
};

enum class Event {
  // внешние ивенты - приходят с контроллера
  kStartE,
  kPauseE,
  kTerminateE,
  kLeftE,
  kRightE,
  kActionOnE,
  kActionOffE,
  // внутренние ивенты - образуются в процессе
  kTickE,
  kCollisionE,
  kMaxLengthE
};

class SnakeFSM {
 public:
  void Fire(Event event);
  void Reset() { state_ = SnakeState::kStart; };
  SnakeState GetState() const { return state_; };

 private:
  SnakeState state_ = SnakeState::kStart;
};
}  // namespace my_nsp