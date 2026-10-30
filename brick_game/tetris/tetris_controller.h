#pragma once

#include "../common/i_game_controller.h"

namespace my_nsp {

class TetrisController : public IGameController {
 public:
  TetrisController();

  void userInput(UserAction_t action, bool hold) override;
  GameInfo_t updateCurrentState() override;
  GameStatus GetStatus() const override;
  int GetNextFigureType() const override;
};

}  // namespace my_nsp