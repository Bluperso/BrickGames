#include "high_score_rep.h"

#include <cstdlib>
#include <fstream>

namespace my_nsp {

HighScoreRepository::HighScoreRepository(std::string path)
    : path_(std::move(path)) {}

int HighScoreRepository::Load() const {
  std::ifstream in(path_);
  if (!in.is_open()) return 0;

  int score = 0;
  in >> score;
  return score > 0 ? score : 0;
}

void HighScoreRepository::Save(int score) const {
  std::ofstream out(path_, std::ios::trunc);
  if (!out.is_open()) return;
  out << score << '\n';
}

std::string DefaultHighScorePath() {
  const char *home = std::getenv("HOME");
  if (home == nullptr) return ".brickgame_snake_highscore";
  return std::string(home) + "/.brickgame_snake_highscore";
}

}  // namespace my_nsp