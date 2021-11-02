#include "xcshell/parser.h"

#include <gtest/gtest.h>

#include <vector>

#include "xcshell/utils.h"

TEST(ParseTest, Parse_PraseStringCorrectly) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "x";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "x");
  EXPECT_EQ(command_parse_result.args, vec);
}

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

TEST(ParseTest, Parse_PraseOutputRedirectionWithSingleBuildInCommand) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "alias";
  std::vector<std::string> vec;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result = command_parse_result_list[0];

  EXPECT_EQ(command_parse_result.command, "alias");
  EXPECT_EQ(command_parse_result.args, vec);
  EXPECT_EQ(command_parse_result.output_redirect_file, "");
  EXPECT_EQ(command_parse_result.input_redirect_file, "");
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

TEST(ParseTest, Parse_CorrectlyParseTwoCommandsWithBuildInCommand) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "alias | grep ls";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  vec_second_command_args.emplace_back("ls");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "alias");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "grep");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
}

TEST(ParseTest, Parse_CorrectlyParseMoreCommandsAtTheSameTime) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "cat /etc/passwd | grep sh | less";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  std::vector<std::string> vec_third_command_args;
  vec_first_command_args.emplace_back("/etc/passwd");
  vec_second_command_args.emplace_back("sh");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];
  CommandParseResult command_parse_result_with_third =
      command_parse_result_list[2];

  EXPECT_EQ(command_parse_result_with_first.command, "cat");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "grep");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_third.command, "less");
  EXPECT_EQ(command_parse_result_with_third.args, vec_third_command_args);
  EXPECT_EQ(command_parse_result_with_third.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_third.input_redirect_file, "");
}

TEST(ParseTest, Parse_PraseStandardtErrorRedirectionCorrectlyWithOverwrite) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo '1 / 0' | bc 2> a.txt";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  std::vector<std::string> vec_third_command_args;
  vec_first_command_args.emplace_back("1 / 0");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "bc");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.error_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result_with_second.output_is_append, false);
  EXPECT_EQ(command_parse_result_with_second.stderr_is_append, false);
}

TEST(ParseTest, Parse_PraseStandardtErrorRedirectionCorrectlyWithAppend) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo '1 / 0' | bc 2>> a.txt";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  std::vector<std::string> vec_third_command_args;
  vec_first_command_args.emplace_back("1 / 0");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "bc");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.error_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result_with_second.output_is_append, false);
  EXPECT_EQ(command_parse_result_with_second.stderr_is_append, true);
}

TEST(ParseTest, Parse_PraseStandardtErrorToStdoutRedirectCorrectWithOverwrite) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo '1 / 0' | bc > a.txt 2>&1";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  std::vector<std::string> vec_third_command_args;
  vec_first_command_args.emplace_back("1 / 0");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "bc");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.output_is_append, false);
  EXPECT_EQ(command_parse_result_with_second.stderr_is_append, false);
}

TEST(ParseTest, Parse_PraseStandardtErrorToStdoutRedirectCorrectWithAppend) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo '1 / 0' | bc >> a.txt 2>&1";
  std::vector<std::string> vec_first_command_args;
  std::vector<std::string> vec_second_command_args;
  std::vector<std::string> vec_third_command_args;
  vec_first_command_args.emplace_back("1 / 0");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];
  CommandParseResult command_parse_result_with_second =
      command_parse_result_list[1];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");

  EXPECT_EQ(command_parse_result_with_second.command, "bc");
  EXPECT_EQ(command_parse_result_with_second.args, vec_second_command_args);
  EXPECT_EQ(command_parse_result_with_second.output_redirect_file, "a.txt");
  EXPECT_EQ(command_parse_result_with_second.input_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_second.output_is_append, true);
  EXPECT_EQ(command_parse_result_with_second.stderr_is_append, true);
}

TEST(ParseTest,
     Parse_PraseStandardtErrorAndOutputRedirectCorrectWithOverwrite) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "./a.out > std.out 2> err.out";
  std::vector<std::string> vec_first_command_args;
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "./a.out");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
  EXPECT_EQ(command_parse_result_with_first.output_redirect_file, "std.out");
  EXPECT_EQ(command_parse_result_with_first.input_redirect_file, "");
  EXPECT_EQ(command_parse_result_with_first.error_redirect_file, "err.out");
  EXPECT_EQ(command_parse_result_with_first.stderr_is_append, false);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputStringWithNoQuote) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $HOME";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = utils::GetHomeDir();
  vec_first_command_args.emplace_back(home_str);
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputStringWithsingleQuote) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo \'$HOME\'";
  std::vector<std::string> vec_first_command_args;

  vec_first_command_args.emplace_back("$HOME");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputStringWithDoubleQuote) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo \"$HOME\"";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = utils::GetHomeDir();
  vec_first_command_args.emplace_back(home_str);
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputStringWithNotExist) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $haha";
  std::vector<std::string> vec_first_command_args;
  vec_first_command_args.emplace_back("");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputDoubleStringAndOneNotExist) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $HOME $haha";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = utils::GetHomeDir();
  vec_first_command_args.emplace_back(home_str);
  vec_first_command_args.emplace_back("");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputQuoteDoubleAndOneNotExist) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo \"$HOME $haha\"";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = utils::GetHomeDir();
  vec_first_command_args.emplace_back(home_str).append(" ");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputSpecialStringWithPrefix) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo 123$HOME";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = "123" + utils::GetHomeDir();
  vec_first_command_args.emplace_back(home_str);
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableIfInputSpecialStringWithPrefixInArg) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $123HOME.123";
  std::vector<std::string> vec_first_command_args;
  vec_first_command_args.emplace_back("HOME.123");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableCorrectIfInputSpecialStringWithSuffix) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $HOME.123";
  std::vector<std::string> vec_first_command_args;
  std::string home_str = utils::GetHomeDir() + ".123";
  vec_first_command_args.emplace_back(home_str);
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

TEST(ParseTest,
     Parse_PraseEnvironmentVarliableIfInputSpecialStringWithSuffixByNoSymbol) {
  BuildIn build_in;
  Parser parser(build_in);
  std::string str = "echo $HOME123";
  std::vector<std::string> vec_first_command_args;
  vec_first_command_args.emplace_back("");
  std::vector<CommandParseResult> command_parse_result_list =
      parser.ParseUserInputLine(str);
  CommandParseResult command_parse_result_with_first =
      command_parse_result_list[0];

  EXPECT_EQ(command_parse_result_with_first.command, "echo");
  EXPECT_EQ(command_parse_result_with_first.args, vec_first_command_args);
}

//TEST(ParseTest, SplitArgs_ShouldReturnEmptyListForEmptyString) {
//  BuildIn build_in;
//  Parser parser(build_in);
//
//  auto parts = utils::SplitArgs("");
//
//  EXPECT_TRUE(parts.empty());
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyFor1Arg) {
//  auto parts = utils::SplitArgs("hello");
//
//  EXPECT_EQ(parts.size(), 1);
//  EXPECT_EQ(parts[0], "hello");
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForIgnoreHeadingSpace) {
//  auto parts = utils::SplitArgs(" hello");
//
//  EXPECT_EQ(parts.size(), 1);
//  EXPECT_EQ(parts[0], "hello");
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForIgnoreTailingSpace) {
//  auto parts = utils::SplitArgs("hello  ");
//
//  EXPECT_EQ(parts.size(), 1);
//  EXPECT_EQ(parts[0], "hello");
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForIgnoreExtraMiddleSpace) {
//  std::vector<std::string> expected = {"hello", "world"};
//  auto parts = utils::SplitArgs("hello  world");
//
//  EXPECT_EQ(parts, expected);
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyFor3Arg) {
//  std::vector<std::string> expected = {"hello", "this", "world"};
//  auto parts = utils::SplitArgs("hello this world");
//
//  EXPECT_EQ(parts, expected);
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForSingleQuotation) {
//  std::vector<std::string> expected = {"hello", "'this \"world'"};
//  auto parts = ("hello 'this \"world'");
//
//  EXPECT_EQ(parts, expected);
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForDoubleQuotation) {
//  std::vector<std::string> expected = {"hello", "this 'world"};
//  auto parts = utils::SplitArgs("hello \"this 'world\"");
//
//  EXPECT_EQ(parts, expected);
//}
//
//TEST(ParseTest, SplitArgs_ShouldReturnCorrectlyForArgWithEquationAndQuotetion) {
//  std::vector<std::string> expected = {"ls='ls -G'"};
//  auto parts = utils::SplitArgs("ls='ls -G'");
//
//  EXPECT_EQ(parts, expected);
//}
