#include "tetris.h"

#include <time.h>

// 7 - фигур, по одному исходному положению
static const int figure_templates[7][4][4] = {{// I
                                               {0, 0, 0, 0},
                                               {1, 1, 1, 1},
                                               {0, 0, 0, 0},
                                               {0, 0, 0, 0}},
                                              {// O
                                               {0, 0, 0, 0},
                                               {0, 1, 1, 0},
                                               {0, 1, 1, 0},
                                               {0, 0, 0, 0}},
                                              {// T
                                               {0, 0, 0, 0},
                                               {0, 1, 0, 0},
                                               {1, 1, 1, 0},
                                               {0, 0, 0, 0}},
                                              {// S
                                               {0, 0, 0, 0},
                                               {0, 1, 1, 0},
                                               {1, 1, 0, 0},
                                               {0, 0, 0, 0}},
                                              {// Z
                                               {0, 0, 0, 0},
                                               {1, 1, 0, 0},
                                               {0, 1, 1, 0},
                                               {0, 0, 0, 0}},
                                              {// J
                                               {0, 0, 0, 0},
                                               {1, 0, 0, 0},
                                               {1, 1, 1, 0},
                                               {0, 0, 0, 0}},
                                              {// L
                                               {0, 0, 0, 0},
                                               {0, 0, 1, 0},
                                               {1, 1, 1, 0},
                                               {0, 0, 0, 0}}};

// Функция инициализации поля
int init_field(Field_t *field, int size_x, int size_y) {
  for (int i = 0; i < size_y; i++) {
    for (int j = 0; j < size_x; j++) {
      field->field[i][j] = 0;
    }
  }
  return 0;
}

// Функция инициализации фигуры
int init_figure(Figure_t *figure, FIGURE type) {
  figure->type = type;
  figure->rot_num = 0;
  figure->x = 3;
  figure->y = 0;
  // копируем базовую фигурку
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      figure->rot[0][i][j] = figure_templates[type][i][j];
  // генерируем остальные повороты
  for (int rot = 1; rot < 4; rot++) {
    rotate_fig(figure->rot[rot - 1], figure->rot[rot]);
  }
  return 0;
}

// Функция проверки помещается ли фигура на поле, не вышла ли она за границы
// по координатам x и y верхнего левого угла
int collision_figure(Field_t *field, Figure_t *figure, int x, int y) {
  int collision = 0;
  int rot = figure->rot_num;
  for (int i = 0; i < 4 && !collision; i++) {
    for (int j = 0; j < 4 && !collision; j++) {
      if (figure->rot[rot][i][j] != 0) {
        int row = y + i;
        int col = x + j;
        if (row < 0 || row >= FIELD_SIZE_Y || col < 0 || col >= FIELD_SIZE_X ||
            field->field[row][col] != 0) {
          collision = 1;
        }
      }
    }
  }
  return collision;
}

// Функция размещения фигурки на поле
int place_figure(Field_t *field, Figure_t *fig) {
  int status = -1;
  if (!collision_figure(field, fig, fig->x, fig->y)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (fig->rot[fig->rot_num][i][j] == 1)
          field->field[fig->y + i][fig->x + j] = fig->type + 1;
      }
    }
    status = 0;
  }
  return status;
}

// Функция поворота фигуры на 90 град.
// Параметры:
// src - текущая фигурка
// dst - повернутая
void rotate_fig(int src[4][4], int dst[4][4]) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      dst[j][3 - i] = src[i][j];  // поворот фигуры по часовой
}

// Функция удаления заполненных линий, возвращает количество удалённых линий
int remove_full_lines(Field_t *field) {
  int lines = 0;
  for (int row = FIELD_SIZE_Y - 1; row >= 0;) {
    int full = 1;
    for (int col = 0; (col < FIELD_SIZE_X) && (full); col++) {
      if (field->field[row][col] == 0) full = 0;
    }
    if (full) {
      // Сдвигаем все строки выше на одну вниз
      for (int r = row; r > 0; r--) {
        for (int c = 0; c < FIELD_SIZE_X; c++) {
          field->field[r][c] = field->field[r - 1][c];
        }
      }
      // инициализируем верхнюю строку ноликами
      for (int c = 0; c < FIELD_SIZE_X; c++) field->field[0][c] = 0;
      lines++;
    } else {
      row--;
    }
  }
  return lines;
}

// Функция подсчета заработанных очков за сожжение линий
int calc_score(int lines) {
  int score = 0;
  if (lines == 1)
    score = 100;
  else if (lines == 2)
    score = 300;
  else if (lines == 3)
    score = 700;
  else if (lines == 4)
    score = 1500;
  return score;
}

// Функция генерации случайной фигуры
FIGURE random_figure() {
  static int first = 1;
  if (first) {
    srand(time(NULL));
    first = 0;
  }
  FIGURE fig = (rand() % 7);
  return fig;
}
