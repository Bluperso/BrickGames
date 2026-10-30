#include <chrono>
#include <memory>
#include <thread>

#include "../../brick_game/common/game_factory.h"
#include "../../brick_game/common/i_game_controller.h"
#include "../../brick_game/common/types.h"
#include "console_view.h"

using namespace my_nsp;

int main() {
  ConsoleView init_view(GameType::kSnake);
  init_view.Init();

  auto choice = init_view.ShowGameMenu();
  if (!choice) {
    init_view.Shutdown();
    return 0;
  }

  auto game = CreateGame(*choice);
  ConsoleView view(*choice);
  view.Init();

  bool running = true;

  while (running) {
    if (auto input = view.PollInput()) {
      auto [action, hold] = *input;
      game->userInput(action, hold);
      if (action == Terminate) running = false;
    }

    GameInfo_t info = game->updateCurrentState();
    GameStatus status = game->GetStatus();

    view.Render(info, game->GetNextFigureType());

    switch (status) {
      case GameStatus::kGameOver:
        view.ShowGameOver(info.score);
        break;
      case GameStatus::kWin:
        view.ShowWin(info.score);
        break;
      case GameStatus::kTerminated:
        running = false;
        break;
      default:
        break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{16});
  }

  view.Shutdown();
  return 0;
}