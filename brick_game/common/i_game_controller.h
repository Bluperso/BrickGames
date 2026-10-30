#pragma once

#include "types.h"

namespace my_nsp {

enum class GameStatus {
  kRunning,
  kPaused,
  kGameOver,
  kWin,
  kTerminated,
};

class IGameController {
 public:
  virtual ~IGameController() = default;
  virtual void userInput(UserAction_t action, bool hold) = 0;
  virtual GameInfo_t updateCurrentState() = 0;
  virtual GameStatus GetStatus() const = 0;
  virtual int GetNextFigureType() const { return 0; }
};

}  // namespace my_nsp