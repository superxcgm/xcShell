#include <gtest/gtest.h>

#include "../include/xcshell/utils.h"

TEST(UtilsTest, ShouldSplitCorrectlyFor1part) {
  auto parts = utils::Split("hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, ShouldSplitCorrectlyFor3part) {
  std::vector<std::string> expected = {"hello", "this", "world"};

  auto parts = utils::Split("hello this world");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, ShouldReturnLastDirectory) {
  auto parts = utils::GetLastDir("/usr/bin");

  EXPECT_EQ(parts, "bin");
}

TEST(UtilsTest, ShouldReturnLastDirectoryIgnoreLastSlash) {
  auto parts = utils::GetLastDir("/usr/bin/");

  EXPECT_EQ(parts, "bin");
}

TEST(UtilsTest, ShouldReturnLastDirectoryForRoot) {
  auto parts = utils::GetLastDir("/");

  EXPECT_EQ(parts, "/");
}

TEST(UtilsTest, ShouldReturnLastDirectoryUnchange) {
  auto parts = utils::GetLastDir("bin");

  EXPECT_EQ(parts, "bin");
}
