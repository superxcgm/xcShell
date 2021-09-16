#include <gtest/gtest.h>

#include <vector>

#include "xcshell/parser.h"

TEST(ParseTest, should_prase_output_redirection_correctly_with_overwrite) {
  Parser parser;
  std::string str = "ls > a.txt";
  std::vector<std::string> vec;
  RedirectElement redirectElement = parser.parseUserInputLine(str);
  std::string command = redirectElement.command;
  std::vector<std::string> args = redirectElement.args;
  std::string outputfilename = redirectElement.output_redirect_file;
  std::string inputfilename = redirectElement.input_redirect_file;
  int output_mode = redirectElement.output_mode;
  int cmd_type = redirectElement.cmd_type;

  EXPECT_EQ(command, "ls");
  EXPECT_EQ(args, vec);
  EXPECT_EQ(outputfilename, "a.txt");
  EXPECT_EQ(inputfilename, "");
  EXPECT_EQ(output_mode, 0);
  EXPECT_EQ(cmd_type, 1);
}
TEST(ParseTest, should_prase_output_redirection_correctly_with_append) {
  Parser parser;
  std::string str = "ls -l >> a.txt";
  std::vector<std::string> vec;
  vec.emplace_back("-l");
  RedirectElement redirectElement = parser.parseUserInputLine(str);

  std::string command = redirectElement.command;
  std::vector<std::string> args = redirectElement.args;
  std::string outputfilename = redirectElement.output_redirect_file;
  std::string inputfilename = redirectElement.input_redirect_file;
  int output_mode = redirectElement.output_mode;
  int cmd_type = redirectElement.cmd_type;

  EXPECT_EQ(command, "ls");
  EXPECT_EQ(args, vec);
  EXPECT_EQ(outputfilename, "a.txt");
  EXPECT_EQ(inputfilename, "");
  EXPECT_EQ(output_mode, 1);
  EXPECT_EQ(cmd_type, 1);
}

TEST(ParseTest, should_prase_input_redirection_correctly) {
  Parser parser;
  std::string str = "bc < a.input";
  std::vector<std::string> vec;
  RedirectElement redirectElement = parser.parseUserInputLine(str);

  std::string command = redirectElement.command;
  std::vector<std::string> args = redirectElement.args;
  std::string outputfilename = redirectElement.output_redirect_file;
  std::string inputfilename = redirectElement.input_redirect_file;
  int output_mode = redirectElement.output_mode;
  int cmd_type = redirectElement.cmd_type;

  EXPECT_EQ(command, "bc");
  EXPECT_EQ(args, vec);
  EXPECT_EQ(outputfilename, "");
  EXPECT_EQ(inputfilename, "a.input");
  EXPECT_EQ(output_mode, 0);
  EXPECT_EQ(cmd_type, 1);
}

TEST(ParseTest, should_prase_input_and_output_redirection_correctly) {
  Parser parser;
  std::string str = "bc < a.input > a.txt";
  std::vector<std::string> vec;
  RedirectElement redirectElement = parser.parseUserInputLine(str);

  std::string command = redirectElement.command;
  std::vector<std::string> args = redirectElement.args;
  std::string outputfilename = redirectElement.output_redirect_file;
  std::string inputfilename = redirectElement.input_redirect_file;
  int output_mode = redirectElement.output_mode;
  int cmd_type = redirectElement.cmd_type;

  EXPECT_EQ(command, "bc");
  EXPECT_EQ(args, vec);
  EXPECT_EQ(outputfilename, "a.txt");
  EXPECT_EQ(inputfilename, "a.input");
  EXPECT_EQ(output_mode, 0);
  EXPECT_EQ(cmd_type, 1);
}
