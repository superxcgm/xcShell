#include <gtest/gtest.h>
#include <vector>

#include "xcshell/parser.h"


TEST(ParseTest, Parse_PraseOutputRedirectionCorrectlyWithOverwrite) {
  Parser parser;
  std::string str = "ls > a.txt";
  std::vector<std::string> vec;
  CommandParseResult command_parse_result = parser.ParseUserInputLine(str);

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
  EXPECT_EQ(command_parse_result.output_is_append, false);
}

TEST(ParseTest, Parse_PraseOutputRedirectionCorrectlyWithAppend) {
  Parser parser;
  std::string str = "ls -l >> a.txt";
  std::vector<std::string> vec;
  vec.emplace_back("-l");
  CommandParseResult command_parse_result = parser.ParseUserInputLine(str);

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
  EXPECT_EQ(command_parse_result.output_is_append, true);
}

TEST(ParseTest, Parse_PraseInputRedirectionCorrectly) {
  Parser parser;
  std::string str = "bc < a.input";
  std::vector<std::string> vec;
  CommandParseResult command_parse_result = parser.ParseUserInputLine(str);

  EXPECT_EQ(command_parse_result.command, "bc");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "");
  EXPECT_EQ(command_parse_result.input_redirect_file, "a.input");
}

TEST(ParseTest, Parse_PraseInputAndOutputRedirectionCorrectly) {
  Parser parser;
  std::string str = "bc < a.input > a.txt";
  std::vector<std::string> vec;
  CommandParseResult command_parse_result = parser.ParseUserInputLine(str);

  EXPECT_EQ(command_parse_result.command, "bc");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "a.input");
  EXPECT_EQ(command_parse_result.output_is_append, false);
}

TEST(ParseTest, Parse_OutputCorrectlyNotContainAnyRedirection) {
  Parser parser;
  std::string str = "ls -a";
  std::vector<std::string> vec;
  vec.emplace_back("-a");
  CommandParseResult command_parse_result = parser.ParseUserInputLine(str);

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
}
