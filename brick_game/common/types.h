#pragma once

namespace my_nsp {

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

enum class GameType {
  kSnake,
  kTetris,
};

}  // namespace my_nsp
