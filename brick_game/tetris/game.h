#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tetris.h"

// // пользовательские кнопки
// typedef enum {
//   Start,
//   Pause,
//   Terminate,
//   Left,
//   Right,
//   Up,
//   Down,
//   Action
// } TetrisAction;

typedef enum {
  TStart,
  TPause,
  TTerminate,
  TLeft,
  TRight,
  TUp,
  TDown,
  TAction
} TetrisAction;

// Состояния КА, что бы контролить взаимодействие
// пользователя с полем
typedef enum {
  START,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER
} GameState_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} TetrisInfo;

// // Структура для передачи данных в интерфейс (спецификация)
// typedef struct {
//   int **field;
//   int **next;
//   int score;
//   int high_score;
//   int level;
//   int speed;
//   int pause;
// } GameInfo_t;

// функции логики игрульки:

// Функция обработки пользовательского действия
void userInput(TetrisAction action, bool hold);

TetrisInfo updateCurrentState();

// Функция заполнения информации по игре
void fill_info();

// Функция инициализации игры
void init_game();

// Функция обработки действия пользователя в состоянии КА - moving
void handle_moving_input(TetrisAction act, bool hold);

// Функция попытки поворота фигуры с отскоком от стенки
int try_rotate_with_kick();

// Функция создания фигурки на поле
void spawn_figure();

void shift_down();

// Функция проверки на состояние завершения игры
int is_game_over();

// Функция проверки на состояние старта игры
int is_start();

// Функция возвращающая тип следующей фигуры
int get_next_type();

void load_high_score();

void save_high_score(int score);

// Функции возвращающие состояние структуры игры и устанавливающие
// значения для полей для тестирования
int get_current_x();

int get_current_y();

int get_pause_state();

GameState_t get_game_state();

int get_current_rot();

int get_fall_frames();

void set_field_cell(int row, int col, int value);

#ifdef __cplusplus
}
#endif

#endif