#include "console_view.h"

#include <ncurses.h>

#include <cstdio>

namespace my_nsp {

constexpr int kFieldWidth = 10;
constexpr int kFieldHeight = 20;
constexpr int kFieldTop = 1;
constexpr int kFieldLeft = 1;
constexpr int kPanelLeft = kFieldLeft + kFieldWidth * 2 + 2;

constexpr int kColorSnake = 8;
constexpr int kColorApple = 9;

ConsoleView::ConsoleView(GameType type) : type_(type) {}

ConsoleView::~ConsoleView() { Shutdown(); }

void ConsoleView::Init() {
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  timeout(0);
  InitColors();
  refresh();
}

void ConsoleView::Shutdown() {
  if (isendwin()) return;
  endwin();
}

void ConsoleView::Render(const GameInfo_t &info, int next_type) {
  erase();
  DrawField(info);
  DrawSidePanel(info);
  if (info.next != nullptr) {
    DrawNext(info.next, next_type);
  }

  if (info.pause) {
    const std::string text = "*** PAUSED ***";
    const int mid_y = kFieldTop + kFieldHeight / 2;
    const int mid_x =
        kFieldLeft + (kFieldWidth * 2 + 2 - static_cast<int>(text.size())) / 2;
    attron(A_BOLD);
    mvprintw(mid_y, mid_x, "%s", text.c_str());
    attroff(A_BOLD);
  }
  refresh();
}

void ConsoleView::InitColors() {
  if (has_colors()) {
    start_color();

    init_pair(1, COLOR_RED, COLOR_RED);          // I
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);    // O
    init_pair(3, COLOR_BLUE, COLOR_BLUE);        // T
    init_pair(4, COLOR_GREEN, COLOR_GREEN);      // S
    init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);  // Z
    init_pair(6, COLOR_CYAN, COLOR_CYAN);        // J
    init_pair(7, COLOR_WHITE, COLOR_WHITE);      // L

    init_pair(kColorSnake, COLOR_GREEN, COLOR_GREEN);
    init_pair(kColorApple, COLOR_RED, COLOR_RED);
  }
}

void ConsoleView::DrawField(const GameInfo_t &info) {
  const int x0 = kFieldLeft;
  const int y0 = kFieldTop;

  mvaddch(y0 - 1, x0 - 1, ACS_ULCORNER);
  for (int i = 0; i < kFieldWidth * 2 + 1; ++i) {
    mvaddch(y0 - 1, x0 + i, ACS_HLINE);
  }
  mvaddch(y0 - 1, x0 + kFieldWidth * 2, ACS_URCORNER);

  for (int y = 0; y < kFieldHeight; ++y) {
    mvaddch(y0 + y, x0 - 1, ACS_VLINE);

    for (int x = 0; x < kFieldWidth; ++x) {
      const int val = info.field[y][x];
      if (val > 0) {
        int pair;
        if (type_ == GameType::kSnake) {
          pair = (val == 1) ? kColorSnake : kColorApple;
        } else {
          pair = val;
        }
        attron(COLOR_PAIR(pair));
        mvprintw(y0 + y, x0 + x * 2, "  ");
        attroff(COLOR_PAIR(pair));
      } else {
        mvprintw(y0 + y, x0 + x * 2, "  ");
      }
    }

    mvaddch(y0 + y, x0 + kFieldWidth * 2, ACS_VLINE);
  }

  mvaddch(y0 + kFieldHeight, x0 - 1, ACS_LLCORNER);
  for (int i = 0; i < kFieldWidth * 2 + 1; ++i) {
    mvaddch(y0 + kFieldHeight, x0 + i, ACS_HLINE);
  }
  mvaddch(y0 + kFieldHeight, x0 + kFieldWidth * 2, ACS_LRCORNER);
}

void ConsoleView::DrawSidePanel(const GameInfo_t &info) {
  int offset = 5;
  mvprintw(kFieldTop + offset, kPanelLeft, "Score:      %d", info.score);
  mvprintw(kFieldTop + offset + 1, kPanelLeft, "High Score: %d",
           info.high_score);
  mvprintw(kFieldTop + offset + 2, kPanelLeft, "Level:      %d", info.level);
  mvprintw(kFieldTop + offset + 3, kPanelLeft, "Speed:      %d", info.speed);

  mvprintw(kFieldTop + offset + 5, kPanelLeft, "[s] start");
  mvprintw(kFieldTop + offset + 6, kPanelLeft, "[p] pause");
  mvprintw(kFieldTop + offset + 7, kPanelLeft, "[q] quit");
  mvprintw(kFieldTop + offset + 8, kPanelLeft, "[<][^][v][>] actions");
}

std::optional<std::pair<UserAction_t, bool>> ConsoleView::PollInput() {
  const int ch = getch();
  if (ch == ERR) return std::nullopt;

  switch (ch) {
    case 's':
    case 'S':
    case '\n':
      return std::make_pair(Start, false);
    case 'p':
    case 'P':
      return std::make_pair(Pause, false);
    case 'q':
    case 'Q':
    case 27:
      return std::make_pair(Terminate, false);
    case KEY_LEFT:
      return std::make_pair(Left, false);
    case KEY_RIGHT:
      return std::make_pair(Right, false);
    case KEY_UP:
      return std::make_pair(Up, false);
    case KEY_DOWN:
      return std::make_pair(Down, false);
    case ' ':
      if (type_ == GameType::kSnake) {
        boost_on_ = !boost_on_;
        return std::make_pair(Action, boost_on_);
      }
      return std::make_pair(Action, true);
    default:
      return std::nullopt;
  }
}

std::optional<GameType> ConsoleView::ShowGameMenu() {
  erase();
  attron(A_BOLD);
  mvprintw(3, 8, "*** BRICKGAME ***");
  attroff(A_BOLD);

  mvprintw(6, 8, "1 - Snake");
  mvprintw(7, 8, "2 - Tetris");
  mvprintw(9, 8, "Press 1, 2, or Q to quit");
  refresh();

  while (true) {
    int ch = getch();
    if (ch == '1') return GameType::kSnake;
    if (ch == '2') return GameType::kTetris;
    if (ch == 'q' || ch == 'Q' || ch == 27) return std::nullopt;
    napms(50);
  }
}

void ConsoleView::ShowGameOver(int score) {
  const int mid_y = kFieldTop + kFieldHeight / 2;
  const int mid_x = kFieldLeft + kFieldWidth / 2;

  attron(A_BOLD);
  mvprintw(mid_y, mid_x, "GAME OVER");
  mvprintw(mid_y + 1, mid_x, "Score: %d", score);
  mvprintw(mid_y + 2, mid_x, "[s] restart");
  mvprintw(mid_y + 3, mid_x, "[q] quit");
  attroff(A_BOLD);
  refresh();
}

void ConsoleView::ShowWin(int score) {
  const int mid_y = kFieldTop + kFieldHeight / 2;
  const int mid_x = kFieldLeft + kFieldWidth / 2;

  attron(A_BOLD);
  mvprintw(mid_y, mid_x, "YOU WIN!");
  mvprintw(mid_y + 1, mid_x, "Score: %d", score);
  mvprintw(mid_y + 2, mid_x, "[s] restart");
  mvprintw(mid_y + 3, mid_x, "[q] quit");
  attroff(A_BOLD);
  refresh();
}

void ConsoleView::DrawNext(int **next, int next_type) {
  const int y0 = kFieldTop + 1;
  const int x0 = kPanelLeft + 1;

  mvprintw(y0 - 1, x0 + 2, "Next:");
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (next[i][j] > 0) {
        attron(COLOR_PAIR(next_type));
        mvprintw(y0 + i, x0 + j * 2, "  ");
        attroff(COLOR_PAIR(next_type));
      } else {
        mvprintw(y0 + i, x0 + j * 2, "  ");
      }
    }
  }
}

}  // namespace my_nsp