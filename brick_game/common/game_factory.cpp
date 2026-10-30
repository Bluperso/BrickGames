#include "game_factory.h"

#include "../snake/snake_controller.h"
#include "../tetris/tetris_controller.h"

namespace my_nsp {

std::unique_ptr<IGameController> CreateGame(GameType type) {
  switch (type) {
    case GameType::kSnake:
      return std::make_unique<SnakeController>();
    case GameType::kTetris:
      return std::make_unique<TetrisController>();
  }
  return nullptr;
}

}  // namespace my_nsp