#pragma once

#include <memory>

#include "i_game_controller.h"
#include "types.h"

namespace my_nsp {

std::unique_ptr<IGameController> CreateGame(GameType type);

}  // namespace my_nsp