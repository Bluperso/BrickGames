#ifndef TETRIS
#define TETRIS

#include <stdlib.h>

#define FIELD_SIZE_Y 20
#define FIELD_SIZE_X 10

typedef enum FIGURE { I, O, T, S, Z, J, L } FIGURE;

typedef struct Figure_t {
  int rot[4][4][4];  // 4 положения фигуры
  int rot_num;       // номер текущего состояния
  int x;  // координаты левого верхнего угла поля фигурки
  int y;
  FIGURE type;  // тип хранимой фигурки (для дальнейшей покраски в нужный цвет)
} Figure_t;

typedef struct Field_t {
  int field[FIELD_SIZE_Y][FIELD_SIZE_X];
  Figure_t figure;
  //   int pos_x;
  //   int pos_y;
} Field_t;

// Функция инициализации поля
int init_field(Field_t *field, int size_x, int size_y);

// Функция инициализации фигуры
int init_figure(Figure_t *figure, FIGURE type);

// Функция проверки помещается ли фигура на поле, не вышла ли она за границы
// по координатам x и y верхнего левого угла
int collision_figure(Field_t *field, Figure_t *figure, int x, int y);

// Функция размещения фигурки на поле
int place_figure(Field_t *field, Figure_t *figure);

// Функция поворота фигуры на 90 град.
// Параметры:
// src - текущая фигурка
// dst - повернутая
void rotate_fig(int src[4][4], int dst[4][4]);

// Функция удаления заполненных линий, возвращает количество удалённых линий
int remove_full_lines(Field_t *field);

// Функция подсчета заработанных очков за сожжение линий
int calc_score(int lines);

// Функция генерации случайной фигуры
FIGURE random_figure();

#endif