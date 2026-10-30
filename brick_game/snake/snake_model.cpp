#include "snake_model.h"

namespace my_nsp {

SnakeModel::SnakeModel(std::chrono::milliseconds tick_interval,
                       std::string high_score_path)
    : high_score_repo_(std::move(high_score_path)),
      tick_interval_(tick_interval),
      rng_(42) {
  for (int y = 0; y < kHeight; ++y) {
    field_rows_[y] = field_[y].data();
  }
  high_score_ = high_score_repo_.Load();
}

void SnakeModel::Start() {
  fsm_.Fire(Event::kStartE);
  if (fsm_.GetState() == SnakeState::kSpawn) {
    Reset();
  }
}

void SnakeModel::TogglePause() {
  fsm_.Fire(Event::kPauseE);
  pause_ = (fsm_.GetState() == SnakeState::kPause);
  if (fsm_.GetState() == SnakeState::kMoving) {
    last_tick_ = std::chrono::steady_clock::now();
  }
}

void SnakeModel::Terminate() { fsm_.Fire(Event::kTerminateE); }

void SnakeModel::TurnLeft() {
  if (fsm_.GetState() != SnakeState::kMoving) return;
  Direction left = LeftOf(snake_.next_direction);
  if (left == Opposite(snake_.direction)) return;
  snake_.next_direction = left;
}

void SnakeModel::TurnRight() {
  if (fsm_.GetState() != SnakeState::kMoving) return;
  Direction right = RightOf(snake_.next_direction);
  if (right == Opposite(snake_.direction)) return;
  snake_.next_direction = right;
}

void SnakeModel::SetDirection(Direction d) {
  if (fsm_.GetState() != SnakeState::kMoving) return;
  if (d == Opposite(snake_.direction)) return;
  if (d == snake_.direction) return;
  snake_.next_direction = d;
}

GameInfo_t SnakeModel::UpdateCurrentState() {
  TickIfTimeElapsed();
  return BuildGameInfo();
}

void SnakeModel::Tick() {
  if (fsm_.GetState() == SnakeState::kSpawn) {
    fsm_.Fire(Event::kTickE);
  } else if (fsm_.GetState() == SnakeState::kMoving) {
    OnTick();
  }
}

void SnakeModel::SetBoost(bool hold) {
  if (fsm_.GetState() != SnakeState::kMoving) return;
  boost_ = hold;
}

void SnakeModel::SetAppleForTest(const Point &p) {
  if (p.x < 0 || p.x >= kWidth || p.y < 0 || p.y >= kHeight) {
    return;
  }
  if (apple_.pos.x >= 0 && apple_.pos.x < kWidth && apple_.pos.y >= 0 &&
      apple_.pos.y < kHeight) {
    field_[apple_.pos.y][apple_.pos.x] = 0;
  }
  apple_.pos = p;
  field_[p.y][p.x] = 2;
}

void SnakeModel::TickIfTimeElapsed() {
  auto now = std::chrono::steady_clock::now();
  auto effective = boost_ ? tick_interval_ / 2 : tick_interval_;
  if (now - last_tick_ >= effective) {
    last_tick_ = now;
    Tick();
  }
}

void SnakeModel::Reset() {
  for (auto &row : field_) row.fill(0);

  score_ = 0;
  level_ = 1;
  pause_ = false;
  boost_ = false;

  snake_.body.clear();
  snake_.direction = Direction::kRight;
  snake_.next_direction = Direction::kRight;

  const int start_x = kWidth / 2;
  const int start_y = kHeight / 2;
  for (int i = 0; i < kStartLength; ++i)
    snake_.body.push_back({start_x - i, start_y});

  for (const auto &p : snake_.body) field_[p.y][p.x] = 1;

  SpawnApple();
  last_tick_ = std::chrono::steady_clock::now();
}

void SnakeModel::OnTick() {
  ApplyDirection();
  MoveSnake();
}

void SnakeModel::ApplyDirection() { snake_.direction = snake_.next_direction; }

void SnakeModel::MoveSnake() {
  // WHY - в общем че тут происходит
  Point head = snake_.body.front();
  switch (snake_.direction) {
    case Direction::kUp:
      head.y -= 1;
      break;
    case Direction::kDown:
      head.y += 1;
      break;
    case Direction::kLeft:
      head.x -= 1;
      break;
    case Direction::kRight:
      head.x += 1;
      break;
  }

  if (CheckWallCollision(head) || CheckSelfCollision(head)) {
    fsm_.Fire(Event::kCollisionE);
    return;
  }

  const bool ate_apple = CheckAppleEaten(head);

  if (!ate_apple) {
    Point tail = snake_.body.back();
    field_[tail.y][tail.x] = 0;
    snake_.body.pop_back();
  } else {
    field_[head.y][head.x] = 0;
  }
  snake_.body.push_front(head);
  field_[head.y][head.x] = 1;

  if (ate_apple) {
    EatApple();
  }

  if (snake_.body.size() >= static_cast<size_t>(kMaxLength)) {
    fsm_.Fire(Event::kMaxLengthE);
  }
}

GameInfo_t SnakeModel::BuildGameInfo() {
  GameInfo_t info{};
  info.field = field_rows_.data();
  info.next = nullptr;
  info.score = score_;
  info.high_score = high_score_;
  info.level = level_;
  info.speed = level_ * 10;
  info.pause = (pause_ ? 1 : 0);
  return info;
}

bool SnakeModel::CheckAppleEaten(const Point &head) const {
  return head == apple_.pos;
}

void SnakeModel::EatApple() {
  ++score_;
  UpdateLevelAndSpeed();
  if (score_ > high_score_) {
    high_score_ = score_;
    high_score_repo_.Save(high_score_);
  }
  SpawnApple();
}

void SnakeModel::SpawnApple() {
  std::array<Point, kWidth * kHeight> empty{};
  int count = 0;
  for (int y = 0; y < kHeight; ++y)
    for (int x = 0; x < kWidth; ++x)
      if (field_[y][x] == 0) empty[count++] = {x, y};

  if (count == 0) return;
  std::uniform_int_distribution<int> dist(0, count - 1);
  apple_.pos = empty[dist(rng_)];
  field_[apple_.pos.y][apple_.pos.x] = 2;
}

void SnakeModel::UpdateLevelAndSpeed() {
  int new_level = 1 + score_ / kPointsPerLevel;
  if (new_level > kMaxLevel) new_level = kMaxLevel;
  if (new_level != level_) {
    level_ = new_level;
    tick_interval_ = std::chrono::milliseconds{500 - (level_ - 1) * 50};
  }
}

bool SnakeModel::CheckWallCollision(const Point &head) const {
  return head.x < 0 || head.x >= kWidth || head.y < 0 || head.y >= kHeight;
}

bool SnakeModel::CheckSelfCollision(const Point &head) const {
  for (const auto &segment : snake_.body) {
    if (segment == head) return true;
  }
  return false;
}
}  // namespace my_nsp