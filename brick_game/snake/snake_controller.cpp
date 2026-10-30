#include "snake_controller.h"

#include "snake_types.h"

namespace my_nsp {

GameInfo_t SnakeController::updateCurrentState() {
  return model_.UpdateCurrentState();
}

void SnakeController::userInput(UserAction_t action, bool hold) {
  switch (action) {
    case Start:
      model_.Start();
      break;
    case Pause:
      model_.TogglePause();
      break;
    case Terminate:
      model_.Terminate();
      break;
    case Up:
      model_.SetDirection(Direction::kUp);
      break;
    case Down:
      model_.SetDirection(Direction::kDown);
      break;
    case Left:
      model_.SetDirection(Direction::kLeft);
      break;
    case Right:
      model_.SetDirection(Direction::kRight);
      break;
    case Action: {
      const SnakeState s = model_.GetState();
      if (s == SnakeState::kStart || s == SnakeState::kGameOver ||
          s == SnakeState::kWin) {
        model_.Start();
      } else if (s == SnakeState::kMoving) {
        model_.SetBoost(hold);
      }
      break;
    }
  }
}

GameStatus SnakeController::GetStatus() const {
  switch (model_.GetState()) {
    case SnakeState::kPause:
      return GameStatus::kPaused;
    case SnakeState::kGameOver:
      return GameStatus::kGameOver;
    case SnakeState::kWin:
      return GameStatus::kWin;
    case SnakeState::kTerminate:
      return GameStatus::kTerminated;
    default:
      return GameStatus::kRunning;
  }
}

}  // namespace my_nsp