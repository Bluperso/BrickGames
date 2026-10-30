#include "info_widget.h"

#include <QLabel>
#include <QVBoxLayout>

namespace my_nsp {

InfoWidget::InfoWidget(QWidget *parent) : QWidget(parent) {
  auto *layout = new QVBoxLayout(this);
  layout->setAlignment(Qt::AlignTop);
  layout->setContentsMargins(16, 16, 16, 16);
  layout->setSpacing(8);

  next_widget_ = new NextWidget(this);
  next_widget_->setVisible(false);
  score_label_ = new QLabel("Score: 0", this);
  high_score_label_ = new QLabel("High Score: 0", this);
  level_label_ = new QLabel("Level: 1", this);
  speed_label_ = new QLabel("Speed: 500", this);
  hint_label_ = new QLabel("Press Space to start", this);
  hint_label_->setWordWrap(true);
  hint_label_->setStyleSheet("color: gray; font-style: italic;");

  QFont f = score_label_->font();
  f.setPointSize(12);
  score_label_->setFont(f);
  high_score_label_->setFont(f);
  level_label_->setFont(f);
  speed_label_->setFont(f);

  layout->addWidget(next_widget_);
  layout->addSpacing(32);
  layout->addWidget(score_label_);
  layout->addWidget(high_score_label_);
  layout->addWidget(level_label_);
  layout->addWidget(speed_label_);
  layout->addSpacing(20);
  layout->addWidget(hint_label_);
}

void InfoWidget::SetScore(int score) {
  score_label_->setText(QString("Score: %1").arg(score));
}

void InfoWidget::SetHighScore(int high_score) {
  high_score_label_->setText(QString("High Score: %1").arg(high_score));
}

void InfoWidget::SetLevel(int level) {
  level_label_->setText(QString("Level: %1").arg(level));
}

void InfoWidget::SetSpeed(int speed) {
  speed_label_->setText(QString("Speed: %1").arg(speed));
}

void InfoWidget::SetHint(const QString &text) { hint_label_->setText(text); }

void InfoWidget::SetNext(int **next, int type) {
  next_widget_->setVisible(next != nullptr);
  next_widget_->SetNext(next, type);
}
}  // namespace my_nsp