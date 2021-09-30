#include "xcshell/utils.h"

#include <gtest/gtest.h>

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

TEST(UtilsTest, SpiltWithSymbol_ShouldReturnInputLineListIfHavePipeSymbol) {
  std::vector<std::string> expected = {"ls -l", "grep 'hello'"};
  auto input_line = utils::SpiltWithSymbol("ls -l | grep 'hello'", "|");
  EXPECT_EQ(input_line, expected);
}

TEST(UtilsTest, SpiltWithSymbol_ShouldReturnInputLineListIfHaveMutilPipe) {
  std::vector<std::string> expected = {"cat /etc/passwd", "grep sh", "less"};
  auto input_line =
      utils::SpiltWithSymbol("cat /etc/passwd | grep sh | less", "|");
  EXPECT_EQ(input_line, expected);
}

TEST(UtilsTest, LeftTrim_ShouldReturnOriginStringIfNoHeadingSpace) {
  char str[] = "hello";
  auto actual = utils::LeftTrim(str);
  const char *expected = str;
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}

TEST(UtilsTest, LeftTrim_ShouldRemoveHeadingSpace) {
  char str[] = "  hello";
  const char *expected = "hello";
  auto actual = utils::LeftTrim(str);
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}

TEST(UtilsTest, RightTrim_ShouldReturnOriginStringIfNoTrailingSpace) {
  char str[] = "hello";
  auto actual = utils::RightTrim(str);
  const char *expected = str;
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}

TEST(UtilsTest, RightTrim_ShouldRemoveTrailingSpace) {
  char str[] = "hello  ";
  auto actual = utils::RightTrim(str);
  const char *expected = "hello";
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}

TEST(UtilsTest, Trim_ShouldReturnOriginStringIfNoHeadingNorTrailingSpace) {
  char str[] = "hello";
  auto actual = utils::Trim(str);
  const char *expected = str;
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}

TEST(UtilsTest, Trim_ShouldRemoveHeadingAndTrailingSpace) {
  char str[] = "  hello  ";
  auto actual = utils::Trim(str);
  const char *expected = "hello";
  EXPECT_TRUE(strcmp(actual, expected) == 0);
}