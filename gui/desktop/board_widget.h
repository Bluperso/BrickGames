#pragma once

#include <QWidget>
#include <array>

#include "../../brick_game/common/i_game_controller.h"
#include "../../brick_game/common/types.h"

namespace my_nsp {

class BoardWidget : public QWidget {
 public:
  explicit BoardWidget(QWidget *parent = nullptr);

  void SetBoard(int **field);
  void SetScore(int score, int high_score, int level);
  void SetStatus(GameStatus status);

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  std::array<std::array<int, 10>, 20> board_{};
  int score_ = 0;
  int high_score_ = 0;
  int level_ = 1;
  GameStatus status_ = GameStatus::kRunning;
};

}  // namespace my_nsp