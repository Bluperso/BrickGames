#pragma once
#include <QWidget>
#include <array>

namespace my_nsp {
class NextWidget : public QWidget {
 public:
  explicit NextWidget(QWidget *parent = nullptr);
  void SetNext(int **next, int type);

 protected:
  void paintEvent(QPaintEvent *) override;

 private:
  std::array<std::array<int, 4>, 4> next_{};
  int type_ = 0;
  bool has_next_ = false;
};
}  // namespace my_nsp