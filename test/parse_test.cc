#include <gtest/gtest.h>

#include <vector>

#include "xcshell/parser.h"


TEST(ParseTest, should_prase_output_redirection_correctly_with_overwrite) {
  Parser parser;
  std::string str = "ls > a.txt";
  std::vector<std::string> vec;
  CommandParseResult commandParseResult = parser.ParseUserInputLine(str);

  EXPECT_EQ(commandParseResult.command, "ls");
  EXPECT_EQ(commandParseResult.args, vec);
  EXPECT_EQ(commandParseResult.output_redirect_file, "a.txt");
  EXPECT_EQ(commandParseResult.input_redirect_file, "");
  EXPECT_EQ(commandParseResult.output_is_append, false);
}

TEST(ParseTest, should_prase_output_redirection_correctly_with_append) {
  Parser parser;
  std::string str = "ls -l >> a.txt";
  std::vector<std::string> vec;
  vec.emplace_back("-l");
  CommandParseResult commandParseResult = parser.ParseUserInputLine(str);

  EXPECT_EQ(commandParseResult.command, "ls");
  EXPECT_EQ(commandParseResult.args, vec);
  EXPECT_EQ(commandParseResult.output_redirect_file, "a.txt");
  EXPECT_EQ(commandParseResult.input_redirect_file, "");
  EXPECT_EQ(commandParseResult.output_is_append, true);
}

TEST(ParseTest, should_prase_input_redirection_correctly) {
  Parser parser;
  std::string str = "bc < a.input";
  std::vector<std::string> vec;
  CommandParseResult commandParseResult = parser.ParseUserInputLine(str);

  EXPECT_EQ(commandParseResult.command, "bc");
  EXPECT_EQ(commandParseResult.args, vec);
  EXPECT_EQ(commandParseResult.output_redirect_file, "");
  EXPECT_EQ(commandParseResult.input_redirect_file, "a.input");
}

TEST(ParseTest, should_prase_input_and_output_redirection_correctly) {
  Parser parser;
  std::string str = "bc < a.input > a.txt";
  std::vector<std::string> vec;
  CommandParseResult commandParseResult = parser.ParseUserInputLine(str);

  EXPECT_EQ(commandParseResult.command, "bc");
  EXPECT_EQ(commandParseResult.args, vec);
  EXPECT_EQ(commandParseResult.output_redirect_file, "a.txt");
  EXPECT_EQ(commandParseResult.input_redirect_file, "a.input");
  EXPECT_EQ(commandParseResult.output_is_append, false);
}

TEST(ParseTest, should_prase_output_correctly_not_contain_any_redirect) {
  Parser parser;
  std::string str = "ls -a";
  std::vector<std::string> vec;
  vec.emplace_back("-a");
  CommandParseResult commandParseResult = parser.ParseUserInputLine(str);

  EXPECT_EQ(commandParseResult.command, "ls");
  EXPECT_EQ(commandParseResult.args, vec);
  EXPECT_EQ(commandParseResult.output_redirect_file, "");
  EXPECT_EQ(commandParseResult.input_redirect_file, "");
}
