# BrickGame

Учебный проект **BrickGame** - коллекция классических аркад (Snake и Tetris)
с единым интерфейсом управления и двумя вариантами отображения:
консольным (ncurses) и десктопным (Qt5).

Проект построен по принципам **MVC** с разделением на слои, применением
паттерна **Factory Method**, **State (FSM)**. Диаграмма **конечного автомата (КА)** приложена (`diagram.jpg`)

## Содержание

- [BrickGame](#brickgame)
	- [Содержание](#содержание)
	- [Возможности](#возможности)
	- [Архитектура](#архитектура)
	- [Структура проекта](#структура-проекта)
	- [Требования](#требования)
	- [Сборка и запуск](#сборка-и-запуск)
		- [Сборка всего](#сборка-всего)
		- [Консольная версия (ncurses)](#консольная-версия-ncurses)
		- [Desktop версия (Qt5)](#desktop-версия-qt5)
		- [Установка в `~/brickgame/bin`](#установка-в-brickgamebin)
		- [Очистка](#очистка)
		- [Архив с исходниками](#архив-с-исходниками)
	- [Управление](#управление)
		- [Общее](#общее)
		- [Snake](#snake)
		- [Tetris](#tetris)
	- [Тестирование и покрытие](#тестирование-и-покрытие)
		- [Запуск тестов](#запуск-тестов)
		- [Отчёт о покрытии](#отчёт-о-покрытии)
	- [Документация](#документация)
	- [Реализованные паттерны](#реализованные-паттерны)

## Возможности

- **Snake** - классическая змейка.
- **Tetris** - классический тетрис.  
  Обе игры реализованы на поле 10×20. Реализованы уровни скорости и сохранение рекорда между сессиями.
- **Два интерфейса**:
  - Консольный (ncurses) - работает в любом терминале.
  - Десктопный (Qt5) - полноценное графическое окно с меню и виджетами.

- **Сохранение рекордов** - в файл `~/.brickgame_<game>_highscore`.
- **Покрытие тестами** - unit-тесты на Google Test для всех функций FSM, модели,
  контроллеров и фабрики.

## Архитектура

Проект разделён на три слоя:

```
GUI
├── gui/console   (ncurses)
└── gui/desktop   (Qt5 Widgets)
        │
        │  IGameController
        ▼
brick_game/common
  GameFactory · IGameController · types.h
        │
        ├── brick_game/snake  (C++20: Model + FSM + Controller)
        └── brick_game/tetris (C11:   game.c + tetris.c + C++ Controller)
```

Ключевые решения:

- **`brick_game/common/types.h`** - общие типы `GameInfo_t`, `UserAction_t`,
  `GameType`, которыми обмениваются GUI и логика.
- **`IGameController`** - абстрактный интерфейс (`userInput`,
  `updateCurrentState`, `GetStatus`, `GetNextFigureType`). GUI работает только
  через него и ничего не знает о конкретной модели игры.
- **`GameFactory`** - статическая функция `CreateGame(GameType)` возвращает
  `std::unique_ptr<IGameController>` нужной игры.
- **Snake** реализован на C++20: модель (`SnakeModel`), конечный автомат
  (`SnakeFSM`) и контроллер (`SnakeController`).
- **Tetris** - ядро на C11 (`tetris.c`, `game.c`), обёрнутое в C++
  контроллер (`TetrisController`), который адаптирует C-API к `IGameController`.
- **КА** для Tetris: `Start → Spawn → Moving ⇄ Pause → GameOver/Win → Terminate`. Более точное описание Snake в `diagram.jpg`

## Структура проекта

```
.
├── Makefile
├── brick_game/
│   ├── common/
│   │   ├── types.h                         # общие типы для gui и model
│   │   ├── i_game_controller.h    # абстрактный интерфейс
│   │   ├── game_factory.h/.cpp    # фабрика игр (выбор какую игру запускать)
│   │   └── game_type.h
│   ├── snake/
│   │   ├── snake_types.h              # константы для поля, Direction, Point
│   │   ├── snake_fsm.h/.cpp         # модель конечного автомата змейки
│   │   ├── snake_model.h/.cpp     # игровая модель
│   │   ├── snake_controller.h/.cpp
│   │   ├── high_score_rep.h/.cpp  # репозиторий под рекорды
│   │   └── tests/                              # unit-тесты (gtest)
│   └── tetris/
│       ├── tetris.h/.c               # фигуры, поле, повороты, линии
│       ├── game.h/.c              # КА тетриса и игровой цикл
│       ├── tetris_controller.h/.cpp
│       └── tests/
└── gui/
    ├── console/
    │   ├── start.cpp              # точка входа консольной версии
    │   └── console_view.h/.cpp    # отрисовка на ncurses
    └── desktop/
        ├── start.cpp                       # точка входа desktop версии
        ├── start_window.h/.cpp    # главное окно
        ├── board_widget.h/.cpp    # игровое поле
        ├── info_widget.h/.cpp      # панель с дополнительной информацией
        └── next_widget.h/.cpp      # превью следующей фигуры (для тетриса)
```

## Требования

| Инструмент | Версия / пакет |
|---|---|
| Компилятор C++ | `g++` с поддержкой C++20 |
| Компилятор C | `gcc` с поддержкой C11 |
| GNU Make | любая актуальная версия |
| ncurses | `libncurses-dev` |
| Qt5 | `qtbase5-dev` (`pkg-config Qt5Widgets`) |
| Google Test | `libgtest-dev` |
| lcov / genhtml | для отчёта о покрытии |
| doxygen | для генерации документации |

Установка на Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential libncurses-dev qtbase5-dev \
                 libgtest-dev lcov doxygen
```

## Сборка и запуск

Все команды выполняются из корня репозитория.

### Сборка всего

```bash
make
```

Собирает три бинарника в `build/`: `tests`, `console`, `desktop`.

### Консольная версия (ncurses)

```bash
make console
```

### Desktop версия (Qt5)

```bash
make desktop
```

### Установка в `~/brickgame/bin`

```bash
make install      # INSTALL_DIR=... для нестандартного пути
make uninstall
```

### Очистка

```bash
make clean
```

### Архив с исходниками

```bash
make dist
```

Создаёт `build/brickgame-YYYYMMDD.tar.gz` без каталога `build`.

## Управление

### Общее

| Действие | Клавиша |
|---|---|
| Старт / рестарт | `S` или `Enter` (консоль), `Space` (Qt) |
| Пауза | `P` |
| Выход | `Q` или `Esc` |
| Движение | стрелками |
| Действие | `Space` |

### Snake

- Стрелки задают направление движения змейки.
- `Space` включает **boost** (удвоенная скорость).
- Поедание яблока: +1 очко, +1 к длине, рекорд сохраняется в файл.
- Каждые 5 очков - новый уровень, интервал тика сокращается на 50 мс.
- Победа - длина змейки достигает 200 клеток (поле заполнено).
- Проигрыш - столкновение со стеной или с собственным телом.

### Tetris

- Стрелки `<- ->` - сдвиг фигуры, `Down` - ускорить падение.
- `Space` - поворот фигуры с «отскоком» от стен.
- За 1/2/3/4 линии - 100/300/700/1500 очков соответственно.
- Уровень растёт каждые 600 очков (максимум 10), скорость увеличивается.
- Проигрыш - новая фигура уже не помещается на поле.

## Тестирование и покрытие

### Запуск тестов

```bash
make test
```

Используется Google Test. Покрыты:

- константы и хелперы направлений (`test_snake_types.cpp`);
- переходы FSM (`test_snake_fsm.cpp`);
- модель змейки - коллизии, еда, уровни, boost (`test_snake_model.cpp`);
- контроллер змейки (`test_snake_controller.cpp`);
- контроллер тетриса (`test_tetris_controller.cpp`);
- фабрика игр (`test_game_factory.cpp`);
- репозиторий рекордов (`test_high_score_rep.cpp`).

### Отчёт о покрытии

```bash
make gcov_report
```

Генерирует HTML-отчёт в `coverage/index.html` (через `lcov` + `genhtml`).
Каталоги `build/`, `tests/` и системные заголовки из покрытия исключены.

## Документация

```bash
make dvi
```

Doxygen генерирует HTML-документацию в `build/docs/html/index.html`
(требуется `Doxyfile` в корне).

## Реализованные паттерны

| Паттерн | Где применяется |
|---|---|
| **Factory Method** | `CreateGame(GameType)` в `game_factory.cpp` |
| **State (FSM)** | `SnakeFSM` (C++) и `game.c` (C) - переходы между состояниями (КА) |
| **MVC** | Модель (`*_model.*` / `game.c`) - контроллер - вид (`gui/*`) |
| **Adapter** | `TetrisController` адаптирует C-API `game.h` к `IGameController` |
