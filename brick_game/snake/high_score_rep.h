#pragma once

#include <string>

namespace my_nsp {

class HighScoreRepository {
 public:
  explicit HighScoreRepository(std::string path);
  int Load() const;
  void Save(int score) const;

 private:
  std::string path_;
};
std::string DefaultHighScorePath();

}  // namespace my_nsp