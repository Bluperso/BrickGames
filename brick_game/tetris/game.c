#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Состояние игры
static struct {
  Field_t field;     // поле
  Figure_t current;  // фигурка текущая
  Figure_t next;     // фигурка следующая
  int score;         // текущий счет
  int high_score;  // максимально набитый score из файла
  int level;       // текущий уровень скорости
  int speed;  // задержка в миллисекундах между тиками
  int fall_frames;  // сколько кадров нужно ждать до падения (speed / 50)
  int frame_count;  // счётчик прошедших кадров
  int pause;        // флаг паузы
  GameState_t state;
} game;

// Указатели на строки поля и следующей фигуры для возврата в TetrisInfo
static int *next_ptrs[4];
static TetrisInfo info;

// Функция заполнения информации по игре
void fill_info() {
  // Создаем временное поле для отрисовки
  static int render_field[FIELD_SIZE_Y][FIELD_SIZE_X];
  static int *render_ptrs[FIELD_SIZE_Y];

  // Копируем туда закрепленное поле
  for (int i = 0; i < FIELD_SIZE_Y; i++) {
    for (int j = 0; j < FIELD_SIZE_X; j++) {
      render_field[i][j] = game.field.field[i][j];
    }
  }

  // Рисуем поверх текущую летящую фигуру
  if (game.state == MOVING || game.state == SHIFTING) {
    int rot = game.current.rot_num;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game.current.rot[rot][i][j] == 1) {
          int row = game.current.y + i;
          int col = game.current.x + j;
          // Проверяем, чтобы не вылезти за границы массива
          if (row >= 0 && row < FIELD_SIZE_Y && col >= 0 &&
              col < FIELD_SIZE_X) {
            render_field[row][col] = game.current.type + 1;
          }
        }
      }
    }
  }

  // Заполняем указатели для возврата в main
  for (int i = 0; i < FIELD_SIZE_Y; i++) {
    render_ptrs[i] = render_field[i];
  }
  info.field = render_ptrs;

  // Следующая фигура
  for (int i = 0; i < 4; i++) {
    next_ptrs[i] = game.next.rot[game.next.rot_num][i];
  }
  info.next = next_ptrs;

  info.score = game.score;
  info.high_score = game.high_score;
  info.level = game.level;
  info.speed = game.speed;
  info.pause = game.pause;
}

void init_game() {
  init_field(&game.field, FIELD_SIZE_X, FIELD_SIZE_Y);
  init_figure(&game.current, random_figure());
  game.current.x = 3;
  game.current.y = 0;
  init_figure(&game.next, random_figure());
  game.score = 0;
  load_high_score();
  game.level = 1;
  game.speed = 1000;                   // 1 секунда (в мс)
  game.fall_frames = game.speed / 50;  // 1000 мс / 50 мс на кадр = 20 кадров
  game.frame_count = 0;
  game.pause = 0;
  game.state = START;
}

// Функция обработки действия пользователя в состоянии КА - moving
void handle_moving_input(TetrisAction act, bool hold) {
  static int hold_counter = 0;
  // сдвиг фигурки влево
  if (act == TLeft) {
    if (!collision_figure(&game.field, &game.current, game.current.x - 1,
                          game.current.y)) {
      game.current.x--;
    }
    // сдвиг фигурки в право
  } else if (act == TRight) {
    if (!collision_figure(&game.field, &game.current, game.current.x + 1,
                          game.current.y)) {
      game.current.x++;
    }
    // опускание фигурки
  } else if (act == TDown) {
    if (!hold) {
      if (!collision_figure(&game.field, &game.current, game.current.x,
                            game.current.y + 1)) {
        game.current.y++;
      }
      hold_counter = 0;
      game.frame_count = 0;
    } else {
      hold_counter++;
      if (hold_counter >= 2) {  // сдвиг на каждый 2-й кадр зажатия
        if (!collision_figure(&game.field, &game.current, game.current.x,
                              game.current.y + 1)) {
          game.current.y++;
        }
        hold_counter = 0;
      }
    }
    // поворот фигурки
  } else if (act == TAction) {
    try_rotate_with_kick();
  }
}

// Функция попытки поворота фигуры с отскоком от стенки
int try_rotate_with_kick() {
  int old_rot = game.current.rot_num;
  int old_x = game.current.x;
  int old_y = game.current.y;
  int new_rot = (old_rot + 1) % 4;

  int kicks_x[] = {0, -1, 1, -2, 2};
  int kicks_y[] = {0, 0, 0, 0, 0};
  int kicks_count = 5;

  game.current.rot_num = new_rot;
  int rotated = 0;  // флаг переворота
  int i = 0;        // итератор
  // Перебираем варианты смещений, пока не найдём подходящий
  while (i < kicks_count && !rotated) {
    game.current.x = old_x + kicks_x[i];
    game.current.y = old_y + kicks_y[i];
    if (!collision_figure(&game.field, &game.current, game.current.x,
                          game.current.y)) {
      rotated = 1;
    }
    i++;
  }
  if (!rotated) {
    game.current.rot_num = old_rot;
    game.current.x = old_x;
    game.current.y = old_y;
  }
  return rotated;
}

// Функция обработки пользовательского действия
void userInput(TetrisAction action, bool hold) {
  // при выборе паузы
  if (action == TPause) {
    game.pause = !game.pause;
    // при выборе выхода
  } else if (action == TTerminate) {
    game.state = GAMEOVER;
  }
  if (!game.pause) {
    if (game.state == START) {
      if (action == TStart) {
        game.state = SPAWN;
      }
      // если игра в состоянии "падения фигуры"
    } else if (game.state == MOVING) {
      // обрабатываем воздействие на фигурку
      handle_moving_input(action, hold);
    } else if (game.state == GAMEOVER) {
      if (action == TStart) {
        init_game();
        game.state = SPAWN;
      }
    }
  }
}

// Функция создания фигурки на поле
void spawn_figure() {
  game.current = game.next;
  game.current.x = 3;
  game.current.y = 0;
  init_figure(&game.next, random_figure());
  if (collision_figure(&game.field, &game.current, game.current.x,
                       game.current.y)) {
    game.state = GAMEOVER;
  } else {
    game.state = MOVING;
  }
}

void shift_down() {
  if (!collision_figure(&game.field, &game.current, game.current.x,
                        game.current.y + 1)) {
    game.current.y++;
    game.state = MOVING;
  } else {
    // Прикрепляем фигуру
    place_figure(&game.field, &game.current);
    int lines = remove_full_lines(&game.field);
    game.score += calc_score(lines);
    if (game.score > game.high_score) {
      game.high_score = game.score;
      save_high_score(game.high_score);
    }
    // Обновление уровня и скорости
    int new_level = game.score / 600 + 1;
    if (new_level > 10) new_level = 10;
    if (new_level != game.level) {
      game.level = new_level;
      game.speed = 1000 / game.level;
      game.fall_frames = game.speed / 50;
      if (game.speed < 100) game.speed = 100;
    }
    game.state = ATTACHING;
  }
}

TetrisInfo updateCurrentState() {
  if (game.pause) {
    fill_info();
  } else if (game.state == GAMEOVER) {
    fill_info();
  } else {
    // Просто увеличиваем счётчик кадров каждый вызов
    game.frame_count++;

    // Если накопилось нужное количество кадров и мы в состоянии падения
    if (game.frame_count >= game.fall_frames && game.state == MOVING) {
      game.frame_count = 0;   // сбрасываем счётчик
      game.state = SHIFTING;  // пора сдвигать фигуру вниз
    }
    // Обработка текущего состояния
    if (game.state == SPAWN) {
      spawn_figure();
    } else if (game.state == SHIFTING) {
      shift_down();
    } else if (game.state == ATTACHING) {
      game.state = SPAWN;
    }
    fill_info();
  }
  return info;
}

// Функция проверки на состояние завершения игры
int is_game_over() { return (game.state == GAMEOVER); }

// Функция проверки на состояние старта игры
int is_start() { return (game.state == START); }

// Возвращаемые значения: 1...7 - для цвета фигур
int get_next_type() { return (game.next.type + 1); }

// Функция загрузки рекорда
void load_high_score() {
  const char *home = getenv("HOME");
  char path[512];
  if (home)
    snprintf(path, sizeof(path), "%s/.brickgame_tetris_highscore", home);
  else
    snprintf(path, sizeof(path), ".brickgame_tetris_highscore");

  FILE *file = fopen(path, "r");
  if (file) {
    if (fscanf(file, "%d", &game.high_score) != 1) game.high_score = 0;
    fclose(file);
  } else {
    game.high_score = 0;
  }
}

// Функция записи рекорда
void save_high_score(int score) {
  const char *home = getenv("HOME");
  char path[512];
  if (home)
    snprintf(path, sizeof(path), "%s/.brickgame_tetris_highscore", home);
  else
    snprintf(path, sizeof(path), ".brickgame_tetris_highscore");
  FILE *file = fopen(path, "w");
  if (file) {
    fprintf(file, "%d", score);
    fclose(file);
  }
}

// Функции возвращающие состояние структуры игры для тестирования
int get_current_x() { return game.current.x; }

int get_current_y() { return game.current.y; }

int get_pause_state() { return game.pause; }

GameState_t get_game_state() { return game.state; }

int get_current_rot() { return game.current.rot_num; }

int get_fall_frames() { return game.fall_frames; }

void set_field_cell(int row, int col, int value) {
  if (row >= 0 && row < FIELD_SIZE_Y && col >= 0 && col < FIELD_SIZE_X)
    game.field.field[row][col] = value;
}