#include "tetris_controller.h"

#include "game.h"

namespace my_nsp {

namespace {

TetrisAction ConvertAction(UserAction_t a) {
  switch (a) {
    case Start:
      return TStart;
    case Pause:
      return TPause;
    case Terminate:
      return TTerminate;
    case Left:
      return TLeft;
    case Right:
      return TRight;
    case Up:
      return TUp;
    case Down:
      return TDown;
    case Action:
      return TAction;
  }
  return TStart;
}

GameStatus ConvertStatus(GameState_t s) {
  switch (s) {
    case START:
      return GameStatus::kRunning;
    case SPAWN:
      return GameStatus::kRunning;
    case MOVING:
      return GameStatus::kRunning;
    case SHIFTING:
      return GameStatus::kRunning;
    case ATTACHING:
      return GameStatus::kRunning;
    case GAMEOVER:
      return GameStatus::kGameOver;
  }
  return GameStatus::kRunning;
}

}  // namespace

TetrisController::TetrisController() { init_game(); }

void TetrisController::userInput(UserAction_t action, bool hold) {
  if (action == Action) {
    GameState_t s = ::get_game_state();
    if (s == START || s == GAMEOVER) {
      if (hold) ::userInput(TStart, false);
      return;
    }
    if (hold) {
      ::userInput(TAction, false);
    }
    return;
  }
  ::userInput(ConvertAction(action), hold);
}

GameInfo_t TetrisController::updateCurrentState() {
  TetrisInfo src = ::updateCurrentState();

  GameInfo_t dst{};
  dst.field = src.field;
  dst.next = src.next;
  dst.score = src.score;
  dst.high_score = src.high_score;
  dst.level = src.level;
  dst.speed = src.speed;
  dst.pause = src.pause;
  return dst;
}

GameStatus TetrisController::GetStatus() const {
  if (::get_pause_state()) {
    return GameStatus::kPaused;
  }
  return ConvertStatus(::get_game_state());
}

int TetrisController::GetNextFigureType() const { return ::get_next_type(); }

}  // namespace my_nsp