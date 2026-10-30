#pragma once

#include "../common/i_game_controller.h"
#include "snake_model.h"

namespace my_nsp {

class SnakeController : public IGameController {
 public:
  explicit SnakeController(
      std::chrono::milliseconds tick_interval = std::chrono::milliseconds{500},
      std::string high_score_path = DefaultHighScorePath())
      : model_(tick_interval, std::move(high_score_path)) {}

  void userInput(UserAction_t action, bool hold) override;
  GameInfo_t updateCurrentState() override;
  GameStatus GetStatus() const override;

 private:
  SnakeModel model_;
};

}  // namespace my_nsp