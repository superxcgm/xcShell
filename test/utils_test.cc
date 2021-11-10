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

TEST(UtilsTest, Split_ShouldReturnInputLineListIfHavePipeSymbol) {
  std::vector<std::string> expected = {"ls -l ", " grep 'hello'"};
  auto input_line = utils::Split("ls -l | grep 'hello'", "|");
  EXPECT_EQ(input_line, expected);
}

TEST(UtilsTest, Split_ShouldReturnInputLineListIfHaveMutilPipe) {
  std::vector<std::string> expected = {"cat /etc/passwd ", " grep sh ",
                                       " less"};
  auto input_line = utils::Split("cat /etc/passwd | grep sh | less", "|");
  EXPECT_EQ(input_line, expected);
}

TEST(UtilsTest, Split_ShouldReturnSingleCharacterIfInputSingleCharacter) {
  std::vector<std::string> expected = {"x"};
  auto input_line = utils::Split("x", "|");
  EXPECT_EQ(input_line, expected);
}

TEST(UtilsTest, LeftTrim_ShouldReturnOriginStringIfNoHeadingSpace) {
  std::string str = "hello";

  auto actual = utils::LeftTrim(str);

  EXPECT_EQ(actual, str);
}

TEST(UtilsTest, LeftTrim_ShouldRemoveHeadingSpace) {
  std::string str = "  hello";
  std::string expected = "hello";

  auto actual = utils::LeftTrim(str);

  EXPECT_EQ(actual, expected);
}
TEST(UtilsTest, RightTrim_ShouldReturnOriginStringIfNoTrailingSpace) {
  std::string str = "hello";

  auto actual = utils::RightTrim(str);

  EXPECT_EQ(actual, str);
}

TEST(UtilsTest, RightTrim_ShouldRemoveTrailingSpace) {
  std::string str = "hello  ";
  std::string expected = "hello";

  auto actual = utils::RightTrim(str);

  EXPECT_EQ(actual, expected);
}

TEST(UtilsTest, Trim_ShouldReturnOriginStringIfNoHeadingNorTrailingSpace) {
  std::string str = "hello";

  auto actual = utils::Trim(str);

  EXPECT_EQ(actual, str);
}

TEST(UtilsTest, Trim_ShouldRemoveHeadingAndTrailingSpace) {
  std::string str = "  hello  ";
  std::string expected = "hello";

  auto actual = utils::Trim(str);

  EXPECT_EQ(actual, expected);
}

TEST(UtilsTest, GenerateTmpFileName_ShouldGenerateFileInTmpDir) {
  auto filename = utils::GenerateTmpFileName();

  EXPECT_EQ(filename.find("/tmp/"), 0);
}

TEST(UtilsTest, GetCdHistoryDir_ShouldReturnFileDir) {
  auto parts = utils::GetDirPath("/usr/bin/cd_history.txt");

  EXPECT_EQ(parts, "/usr/bin");
}

TEST(UtilsTest, GetCdHistoryDir_ShouldReturnFileDirWhenInputRootDir) {
  auto parts = utils::GetDirPath("/cd_history.txt");

  EXPECT_EQ(parts, "");
}

TEST(UtilsTest, GetCdHistoryDir_ShouldReturnFileDirWhenInputHomeDir) {
  auto parts = utils::GetDirPath("~/cd_history.txt");

  EXPECT_EQ(parts, "~");
}
