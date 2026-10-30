#pragma once

#include <QWidget>

#include "next_widget.h"

class QLabel;

namespace my_nsp {

class NextWidget;

class InfoWidget : public QWidget {
 public:
  explicit InfoWidget(QWidget *parent = nullptr);

  void SetScore(int score);
  void SetHighScore(int high_score);
  void SetLevel(int level);
  void SetSpeed(int speed);
  void SetHint(const QString &text);
  void SetNext(int **next, int type);

 private:
  NextWidget *next_widget_;
  QLabel *score_label_;
  QLabel *high_score_label_;
  QLabel *level_label_;
  QLabel *speed_label_;
  QLabel *hint_label_;
};

}  // namespace my_nsp