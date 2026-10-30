#include "snake_fsm.h"

namespace my_nsp {
void SnakeFSM::Fire(Event event) {
  switch (state_) {
    case SnakeState::kStart:
      if (event == Event::kStartE) {
        state_ = SnakeState::kSpawn;
      } else if (event == Event::kTerminateE) {
        state_ = SnakeState::kTerminate;
      }
      break;

    case SnakeState::kSpawn:
      if (event == Event::kTickE) {
        state_ = SnakeState::kMoving;
      } else if (event == Event::kTerminateE) {
        state_ = SnakeState::kTerminate;
      }
      break;

    case SnakeState::kMoving:
      switch (event) {
        case Event::kPauseE:
          state_ = SnakeState::kPause;
          break;
        case Event::kCollisionE:
          state_ = SnakeState::kGameOver;
          break;
        case Event::kMaxLengthE:
          state_ = SnakeState::kWin;
          break;
        case Event::kTerminateE:
          state_ = SnakeState::kTerminate;
          break;
        default:
          // kLeftE, kRightE, kActionOnE, kActionOffE, kTickE - состояние не
          // меняют
          break;
      }
      break;

    case SnakeState::kPause:
      if (event == Event::kPauseE) {
        state_ = SnakeState::kMoving;
      } else if (event == Event::kTerminateE) {
        state_ = SnakeState::kTerminate;
      }
      break;

    case SnakeState::kGameOver:
    case SnakeState::kWin:
      if (event == Event::kStartE) {
        state_ = SnakeState::kSpawn;
      } else if (event == Event::kTerminateE) {
        state_ = SnakeState::kTerminate;
      }
      break;

    case SnakeState::kTerminate:
      // терминальное состояние — переходов нет
      break;
  }
}
}  // namespace my_nsp
