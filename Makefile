CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror \
            -I src/brick_game/common \
            -I src/brick_game/snake \
			-I src/brick_game/tetris

LDFLAGS  := -lgtest -lgtest_main -pthread

QT_CXXFLAGS  := -fPIC $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS      := $(shell pkg-config --libs Qt5Widgets)
NCURSES_LIBS := -lncurses

BUILD        := build
COVERAGE_DIR := coverage

# =========== Линк С тетриса ============
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror

TETRIS_OBJ := $(BUILD)/tetris.o $(BUILD)/game.o

$(BUILD)/tetris.o: brick_game/tetris/tetris.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/game.o: brick_game/tetris/game.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@
# =======================================

TEST_SRC := brick_game/snake/tests/test_snake_types.cpp \
            brick_game/snake/tests/test_snake_fsm.cpp \
            brick_game/snake/tests/test_snake_model.cpp \
            brick_game/snake/tests/test_snake_controller.cpp \
            brick_game/snake/tests/test_high_score_rep.cpp \
            brick_game/snake/tests/test_tetris_controller.cpp \
            brick_game/snake/tests/test_game_factory.cpp \
            brick_game/snake/snake_fsm.cpp \
            brick_game/snake/snake_model.cpp \
            brick_game/snake/snake_controller.cpp \
            brick_game/snake/high_score_rep.cpp \
            brick_game/tetris/tetris_controller.cpp \
            $(TETRIS_OBJ) \
            brick_game/common/game_factory.cpp

CONSOLE_SRC := gui/console/start.cpp \
               gui/console/console_view.cpp \
               brick_game/snake/snake_model.cpp \
               brick_game/snake/snake_fsm.cpp \
               brick_game/snake/snake_controller.cpp \
               brick_game/snake/high_score_rep.cpp \
			   brick_game/tetris/tetris_controller.cpp \
			   $(TETRIS_OBJ) \
               brick_game/common/game_factory.cpp

DESKTOP_SRC := gui/desktop/start.cpp \
               gui/desktop/start_window.cpp \
               gui/desktop/board_widget.cpp \
               gui/desktop/info_widget.cpp \
			   gui/desktop/next_widget.cpp \
               brick_game/snake/snake_model.cpp \
               brick_game/snake/snake_fsm.cpp \
               brick_game/snake/snake_controller.cpp \
               brick_game/snake/high_score_rep.cpp \
			   brick_game/tetris/tetris_controller.cpp \
			   $(TETRIS_OBJ) \
               brick_game/common/game_factory.cpp

INSTALL_DIR ?= $(HOME)/brickgame


.PHONY: all build test console desktop install uninstall clean dvi dist gcov_report format

all: $(BUILD)/tests $(BUILD)/console $(BUILD)/desktop

$(BUILD)/tests: $(TEST_SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $@ $(LDFLAGS)

$(BUILD)/console: $(CONSOLE_SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(CONSOLE_SRC) -o $@ $(NCURSES_LIBS)

$(BUILD)/desktop: $(DESKTOP_SRC) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) $(DESKTOP_SRC) -o $@ $(QT_LIBS)

test: $(BUILD)/tests
	./$(BUILD)/tests

console: $(BUILD)/console
	./$(BUILD)/console

desktop: $(BUILD)/desktop
	./$(BUILD)/desktop

install: all
	mkdir -p $(INSTALL_DIR)/bin
	cp $(BUILD)/console $(INSTALL_DIR)/bin/
	cp $(BUILD)/desktop $(INSTALL_DIR)/bin/
	@echo "Installed to $(INSTALL_DIR)/bin"

uninstall:
	rm -f $(INSTALL_DIR)/bin/console
	rm -f $(INSTALL_DIR)/bin/desktop
	@echo "Uninstalled from $(INSTALL_DIR)/bin"

dvi:
	@if ! command -v doxygen >/dev/null 2>&1; then \
	  echo "Error: doxygen is not installed."; \
	  echo "Install with: sudo apt install doxygen"; \
	  exit 1; \
	fi
	@if [ ! -f Doxyfile ]; then \
	  echo "Error: Doxyfile not found in $(CURDIR)."; \
	  exit 1; \
	fi
	@mkdir -p $(BUILD)/docs
	doxygen Doxyfile
	@echo ""
	@echo "Documentation generated: $(BUILD)/docs/html/index.html"

dist: clean
	mkdir -p $(BUILD)
	tar --exclude='./build' \
	    --exclude='./.git' \
	    --exclude='./misc' \
	    -czf $(BUILD)/brickgame-$(shell date +%Y%m%d).tar.gz .
	@echo "Archive: $(BUILD)/brickgame-$(shell date +%Y%m%d).tar.gz"

gcov_report: clean $(TETRIS_OBJ)
	@mkdir -p $(BUILD) $(COVERAGE_DIR)
	$(CXX) $(CXXFLAGS) --coverage $(TEST_SRC) -o $(BUILD)/tests_cov $(LDFLAGS)
	./$(BUILD)/tests_cov
	lcov --capture --directory . --output-file $(COVERAGE_DIR)/coverage.info --ignore-errors mismatch
	lcov --remove $(COVERAGE_DIR)/coverage.info \
	    '/usr/*' '*/tests/*' '*/build/*' \
	    --output-file $(COVERAGE_DIR)/coverage.info --ignore-errors unused
	genhtml $(COVERAGE_DIR)/coverage.info --output-directory $(COVERAGE_DIR)
	@echo ""
	@echo "Coverage report: $(COVERAGE_DIR)/index.html"

format:
	find . -type f \( -name '*.cpp' -o -name '*.c' -o -name '*.h' \) \
	    -not -path './build/*' \
	    -exec clang-format -i {} +
	@echo "Formatted all .cpp/.c/.h files"

clean:
	rm -rf $(BUILD)
	rm -rf $(COVERAGE_DIR)
	find . -name '*.gcda' -delete
	find . -name '*.gcno' -delete

$(BUILD):
	mkdir -p $(BUILD)