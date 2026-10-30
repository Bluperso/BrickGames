#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "../../brick_game/common/game_factory.h"
#include "../../brick_game/common/i_game_controller.h"
#include "board_widget.h"
#include "info_widget.h"

namespace my_nsp {

class MainWindow : public QMainWindow {
 public:
  MainWindow();

 protected:
  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

 private:
  void CreateMenu();
  void StartGame(GameType type);
  void OnTick();
  void UpdateHint();

  bool started_ = false;
  QString last_hint_;
  std::unique_ptr<IGameController> game_;
  BoardWidget *board_;
  InfoWidget *info_;
  QTimer *timer_;
  GameType current_type_ = GameType::kSnake;
  GameStatus last_status_ = GameStatus::kRunning;
};

}  // namespace my_nsp