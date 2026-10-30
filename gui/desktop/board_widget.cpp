#include "board_widget.h"

#include <QPainter>

namespace my_nsp {

namespace {
constexpr int kCols = 10;
constexpr int kRows = 20;
constexpr int kCellSize = 24;  // пикселей на клетку
}  // namespace

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent) {
  setFixedSize(kCols * kCellSize, kRows * kCellSize);
  setStyleSheet("background-color: black;");
}

void BoardWidget::SetBoard(int **field) {
  for (int y = 0; y < 20; ++y)
    for (int x = 0; x < 10; ++x) board_[y][x] = field[y][x];
  update();
}

void BoardWidget::SetScore(int score, int high_score, int level) {
  score_ = score;
  high_score_ = high_score;
  level_ = level;
  update();
}

void BoardWidget::SetStatus(GameStatus status) {
  status_ = status;
  update();
}

void BoardWidget::paintEvent(QPaintEvent *) {
  static const QColor kColors[8] = {
      Qt::black,    // пусто
      Qt::green,    // Snake: змейка;  Tetris: I
      Qt::red,      // Snake: яблоко;  Tetris: O
      Qt::yellow,   // Tetris: T
      Qt::blue,     // Tetris: S
      Qt::magenta,  // Tetris: Z
      Qt::cyan,     // Tetris: J
      Qt::white     // Tetris: L
  };

  QPainter p(this);
  for (int y = 0; y < kRows; ++y) {
    for (int x = 0; x < kCols; ++x) {
      int val = board_[y][x];
      if (val == 0 || val > 7) continue;
      p.fillRect(x * kCellSize, y * kCellSize, kCellSize - 1, kCellSize - 1,
                 kColors[val]);
    }
  }

  p.setPen(Qt::white);
  p.setFont(QFont("Arial", 16, QFont::Bold));

  if (status_ == GameStatus::kPaused) {
    p.drawText(rect(), Qt::AlignCenter, "PAUSED");
  } else if (status_ == GameStatus::kGameOver) {
    p.drawText(rect(), Qt::AlignCenter,
               "GAME OVER\nScore: " + QString::number(score_));
  } else if (status_ == GameStatus::kWin) {
    p.drawText(rect(), Qt::AlignCenter,
               "YOU WIN!\nScore: " + QString::number(score_));
  }
}

}  // namespace my_nsp