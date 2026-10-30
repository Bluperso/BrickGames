#include "start_window.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>

namespace my_nsp {

MainWindow::MainWindow() {
  CreateMenu();
  board_ = new BoardWidget(this);
  info_ = new InfoWidget(this);

  auto *central = new QWidget(this);
  auto *layout = new QHBoxLayout(central);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(board_);
  layout->addWidget(info_);
  setCentralWidget(central);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::OnTick);
  timer_->start(50);

  setFocusPolicy(Qt::StrongFocus);
  StartGame(GameType::kSnake);
}

void MainWindow::CreateMenu() {
  QMenu *game_menu = menuBar()->addMenu("&Game...");

  QAction *snake_action = game_menu->addAction("&Snake");
  QAction *tetris_action = game_menu->addAction("&Tetris");
  game_menu->addSeparator();
  QAction *quit_action = game_menu->addAction("&Quit");

  connect(snake_action, &QAction::triggered, this,
          [this] { StartGame(GameType::kSnake); });
  connect(tetris_action, &QAction::triggered, this,
          [this] { StartGame(GameType::kTetris); });
  connect(quit_action, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::StartGame(GameType type) {
  game_ = CreateGame(type);
  if (!game_) return;

  current_type_ = type;
  started_ = false;
  last_status_ = GameStatus::kRunning;
  setWindowTitle(type == GameType::kSnake ? "BrickGame: Snake"
                                          : "BrickGame: Tetris");
  UpdateHint();
}

void MainWindow::OnTick() {
  if (!game_) return;

  GameInfo_t info = game_->updateCurrentState();
  board_->SetBoard(info.field);
  board_->SetStatus(game_->GetStatus());
  board_->SetScore(info.score, info.high_score, info.level);

  info_->SetScore(info.score);
  info_->SetHighScore(info.high_score);
  info_->SetLevel(info.level);
  info_->SetSpeed(info.speed);
  info_->SetNext(info.next, game_->GetNextFigureType());

  UpdateHint();
}

void MainWindow::UpdateHint() {
  if (!game_) return;

  GameStatus status = game_->GetStatus();
  QString hint;

  if (status == GameStatus::kGameOver || status == GameStatus::kWin) {
    hint = "Press Space to restart\nPress Q or Esc to quit";
  } else if (!started_) {
    hint = "Press Space to start";
  } else {
    hint =
        "Controls:\n"
        "Arrows - move\n"
        "Space - rotate\n"
        "P - pause\n"
        "Q or Esc - quit";
  }

  if (hint != last_hint_) {
    info_->SetHint(hint);
    last_hint_ = hint;
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (!game_) return;

  switch (event->key()) {
    case Qt::Key_Up:
      game_->userInput(Up, false);
      break;
    case Qt::Key_Down:
      game_->userInput(Down, false);
      break;
    case Qt::Key_Left:
      game_->userInput(Left, false);
      break;
    case Qt::Key_Right:
      game_->userInput(Right, false);
      break;
    case Qt::Key_Space:
      game_->userInput(Action, true);
      started_ = true;
      break;
    case Qt::Key_P:
      game_->userInput(Pause, false);
      break;
    case Qt::Key_Q:
    case Qt::Key_Escape:
      game_->userInput(Terminate, false);
      close();
      break;
    default:
      QMainWindow::keyPressEvent(event);
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (!game_) return;
  if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
    game_->userInput(Action, false);
  } else {
    QMainWindow::keyReleaseEvent(event);
  }
}

}  // namespace my_nsp