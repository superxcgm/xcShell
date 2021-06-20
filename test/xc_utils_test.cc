#include "../include/xc_utils.h"

#include <gtest/gtest.h>

TEST(XcUtilsTest, ShouldSplitCorrectlyFor1part) {
  auto parts = xc_utils::Split("hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(XcUtilsTest, ShouldSplitCorrectlyFor3part) {
  std::vector<std::string> expected = {"hello", "this", "world"};

  auto parts = xc_utils::Split("hello this world");

  EXPECT_EQ(parts, expected);
}

TEST(XcUtilsTest, ShouldReturnLastDirectory) {
  auto parts = xc_utils::GetLastDir("/usr/bin");

  EXPECT_EQ(parts, "bin");
}

TEST(XcUtilsTest, ShouldReturnLastDirectoryIgnoreLastSlash) {
  auto parts = xc_utils::GetLastDir("/usr/bin/");

  EXPECT_EQ(parts, "bin");
}

TEST(XcUtilsTest, ShouldReturnLastDirectoryForRoot) {
  auto parts = xc_utils::GetLastDir("/");

  EXPECT_EQ(parts, "/");
}

TEST(XcUtilsTest, ShouldReturnLastDirectoryUnchange) {
  auto parts = xc_utils::GetLastDir("bin");

  EXPECT_EQ(parts, "bin");
}
