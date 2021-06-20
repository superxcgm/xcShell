#include "../include/xc_shell.h"

#include <gtest/gtest.h>

std::istringstream generateIs(size_t len);

// todo: need to handle input and output of child process
// TEST(XcShellTest, ShouldPrintWhatUserInput) {
//  std::string expected = "> hello world\n> ";
//  XcShell xc_shell;
//  std::istringstream is("echo hello world\n");
//  std::ostringstream os;
//
//  xc_shell.process(is, os, std::cerr);
//
//  EXPECT_EQ(expected, os.str());
//}
//
// TEST(XcShellTest, ShouldSupportLongLineLike4096) {
//  XcShell xc_shell;
//  std::istringstream is = generateIs(4096);
//  std::ostringstream os;
//
//  xc_shell.process(is, os, std::cerr);
//
//  EXPECT_GT(os.str().length(), 4096);
//}

std::istringstream generateIs(size_t len) {
  std::stringstream ss;
  ss << "echo ";
  for (int i = 0; i < len; i++) {
    ss << "x";
  }
  return std::istringstream(ss.str());
}
