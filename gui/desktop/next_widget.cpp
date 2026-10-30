#include "next_widget.h"

#include <QPainter>

namespace my_nsp {
namespace {
constexpr int kCell = 24;
const QColor kColors[8] = {
    Qt::black,    // 0 — не используется
    Qt::green,    // 1 — I
    Qt::red,      // 2 — O
    Qt::yellow,   // 3 — T
    Qt::blue,     // 4 — S
    Qt::magenta,  // 5 — Z
    Qt::cyan,     // 6 — J
    Qt::white     // 7 — L
};
}  // namespace

NextWidget::NextWidget(QWidget *parent) : QWidget(parent) {
  setFixedSize(4 * kCell, 4 * kCell);
  setStyleSheet("background-color: black;");
}

void NextWidget::SetNext(int **next, int type) {
  has_next_ = (next != nullptr);
  type_ = type;
  if (has_next_) {
    for (int y = 0; y < 4; ++y)
      for (int x = 0; x < 4; ++x) next_[y][x] = next[y][x];
  }
  update();
}

void NextWidget::paintEvent(QPaintEvent *) {
  if (!has_next_) return;
  QPainter p(this);
  const QColor color = (type_ > 0 && type_ <= 7) ? kColors[type_] : Qt::gray;
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (next_[y][x] != 0) {
        p.fillRect(x * kCell, y * kCell, kCell - 1, kCell - 1, color);
      }
    }
  }
}
}  // namespace my_nsp