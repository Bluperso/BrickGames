#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "../high_score_rep.h"

namespace my_nsp {
namespace {

std::string TempPath(const std::string &name) {
  return (std::filesystem::temp_directory_path() / name).string();
}

TEST(HighScoreRepositoryTest, LoadReturnsZeroIfFileMissing) {
  HighScoreRepository repo{TempPath("my_nsp_nonexistent_file_xyz.txt")};
  EXPECT_EQ(repo.Load(), 0);
}

TEST(HighScoreRepositoryTest, SaveThenLoad) {
  const std::string path = TempPath("my_nsp_hs_test_save_load.txt");
  std::remove(path.c_str());

  HighScoreRepository repo{path};
  repo.Save(42);
  EXPECT_EQ(repo.Load(), 42);

  std::remove(path.c_str());
}

TEST(HighScoreRepositoryTest, SaveOverwrites) {
  const std::string path = TempPath("my_nsp_hs_test_overwrite.txt");
  std::remove(path.c_str());

  HighScoreRepository repo{path};
  repo.Save(10);
  repo.Save(99);
  EXPECT_EQ(repo.Load(), 99);

  std::remove(path.c_str());
}

TEST(HighScoreRepositoryTest, LoadIgnoresGarbage) {
  const std::string path = TempPath("my_nsp_hs_test_garbage.txt");
  {
    std::ofstream out(path);
    out << "not a number";
  }

  HighScoreRepository repo{path};
  EXPECT_EQ(repo.Load(), 0);

  std::remove(path.c_str());
}

}  // namespace
}  // namespace my_nsp