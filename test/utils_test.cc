#include <gtest/gtest.h>

#include "xcshell/utils.h"

TEST(UtilsTest, Split_ShouldSplitCorrectlyFor1part) {
  auto parts = utils::Split("hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, Split_ShouldSplitCorrectlyFor3part) {
  std::vector<std::string> expected = {"hello", "this", "world"};

  auto parts = utils::Split("hello this world");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, GetLastDir_ShouldReturnLastDirectory) {
  auto parts = utils::GetLastDir("/usr/bin");

  EXPECT_EQ(parts, "bin");
}

TEST(UtilsTest, GetLastDir_ShouldReturnLastDirectoryIgnoreLastSlash) {
  auto parts = utils::GetLastDir("/usr/bin/");

  EXPECT_EQ(parts, "bin");
}

TEST(UtilsTest, GetLastDir_ShouldReturnLastDirectoryForRoot) {
  auto parts = utils::GetLastDir("/");

  EXPECT_EQ(parts, "/");
}

TEST(UtilsTest, GetLastDir_ShouldReturnLastDirectoryUnchange) {
  auto parts = utils::GetLastDir("bin");

  EXPECT_EQ(parts, "bin");
}

TEST(UtilsTest, SplitArgs_ShouldReturnEmptyListForEmptyString) {
  auto parts = utils::SplitArgs("");

  EXPECT_TRUE(parts.empty());
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyFor1Arg) {
  auto parts = utils::SplitArgs("hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForIgnoreHeadingSpace) {
  auto parts = utils::SplitArgs(" hello");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForIgnoreTailingSpace) {
  auto parts = utils::SplitArgs("hello  ");

  EXPECT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "hello");
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForIgnoreExtraMiddleSpace) {
  std::vector<std::string> expected = {"hello", "world"};
  auto parts = utils::SplitArgs("hello  world");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyFor3Arg) {
  std::vector<std::string> expected = {"hello", "this", "world"};
  auto parts = utils::SplitArgs("hello this world");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForSingleQuotation) {
  std::vector<std::string> expected = {"hello", "this \"world"};
  auto parts = utils::SplitArgs("hello 'this \"world'");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForDoubleQuotation) {
  std::vector<std::string> expected = {"hello", "this 'world"};
  auto parts = utils::SplitArgs("hello \"this 'world\"");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, SplitArgs_ShouldReturnCorrectlyForArgWithEquationAndQuotetion) {
  std::vector<std::string> expected = {"ls='ls -G'"};
  auto parts = utils::SplitArgs("ls='ls -G'");

  EXPECT_EQ(parts, expected);
}

TEST(UtilsTest, RemoveQuote_ShouldReturnOriginStrIfNoQuote) {
  EXPECT_EQ(utils::RemoveQuote("hehe"), "hehe");
}

TEST(UtilsTest, RemoveQuote_ShouldReturnStringInsideSingleQuote) {
  EXPECT_EQ(utils::RemoveQuote("'hehe'"), "hehe");
}

TEST(UtilsTest, ExpandPath_ShouldReturnOriginPathIfNotStartWithTilde) {
  EXPECT_EQ(utils::ExpandPath("a.txt"), "a.txt");
}

TEST(UtilsTest, ExpandPath_ShouldReturnReplacedPathIfStartWithTilde) {
  auto replaced = utils::ExpandPath("~/a.txt");
  EXPECT_TRUE(replaced.find("~") == std::string::npos);
}
