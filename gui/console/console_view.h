#pragma once

#include <optional>
#include <string>

#include "../../brick_game/common/i_game_controller.h"
#include "../../brick_game/common/types.h"

namespace my_nsp {

class ConsoleView {
 public:
  explicit ConsoleView(GameType type);
  ~ConsoleView();
  void Init();
  void Shutdown();
  void Render(const GameInfo_t &info, int next_type);
  void DrawNext(int **next, int next_type);
  std::optional<std::pair<UserAction_t, bool>> PollInput();
  std::optional<GameType> ShowGameMenu();
  void ShowGameOver(int score);
  void ShowWin(int score);

 private:
  bool boost_on_ = false;
  GameType type_;
  void InitColors();
  void DrawField(const GameInfo_t &info);
  void DrawSidePanel(const GameInfo_t &info);
};

}  // namespace my_nsp