#include "../include/xc_utils.h"

#include <gtest/gtest.h>

TEST(XcUtilsTest, ShouldSplitCorrectlyFor1part) {
  auto parts = xc_utils::split("hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(XcUtilsTest, ShouldSplitCorrectlyFor3part) {
  std::vector<std::string> expected = {"hello", "this", "world"};

  auto parts = xc_utils::split("hello this world");

  EXPECT_EQ(parts, expected);
}
