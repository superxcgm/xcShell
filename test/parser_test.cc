#include "xcshell/parser.h"

#include <gtest/gtest.h>

#include <vector>

TEST(ParseTest, Parse_PraseOutputRedirectionCorrectlyWithOverwrite) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "ls > a.txt";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
  EXPECT_EQ(command_parse_result.output_is_append, false);
}

TEST(ParseTest, Parse_PraseOutputRedirectionCorrectlyWithAppend) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "ls -l >> a.txt";
  std::vector<std::string> vec;
  vec.emplace_back("-l");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
  EXPECT_EQ(command_parse_result.output_is_append, true);
}

TEST(ParseTest, Parse_PraseInputRedirectionCorrectly) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "bc < a.input";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "bc");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "");
  EXPECT_EQ(command_parse_result.input_redirect_file, "a.input");
}

TEST(ParseTest, Parse_PraseInputAndOutputRedirectionCorrectly) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "bc < a.input > a.txt";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "bc");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "a.input");
  EXPECT_EQ(command_parse_result.output_is_append, false);
}

TEST(ParseTest, Parse_OutputCorrectlyNotContainAnyRedirection) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "ls -a";
  std::vector<std::string> vec;
  vec.emplace_back("-a");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "ls");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
}

TEST(ParseTest, Parse_OutputCorrectlyredirectionWithNoCommand) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "> a.txt";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
}

TEST(ParseTest, Parse_CorrectlyParseTwoCommandsAtTheSameTime) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "ls | grep PM";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  vec_second_command_args.emplace_back("PM");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "ls");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "grep");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
}

TEST(ParseTest, Parse_CorrectlyParseTwoCommandsWithAnotherIsNotCommand) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "ls | > a.txt";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;

  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "ls");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result_with_second.output_is_append, false);
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
}
