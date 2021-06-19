#include <gtest/gtest.h>

#include "../include/xc_shell.h"

std::istringstream generateIs(size_t len);

TEST(XcShellTest, ShouldPrintWhatUserInput) {
  std::string expected = "> hello world\n> \n";
  XcShell xc_shell;
  std::istringstream is("hello world\n");
  std::ostringstream os;

  xc_shell.process(is, os);

  EXPECT_EQ(expected, os.str());
}

TEST(XcShellTest, ShouldSupportLongLineLike4096) {
  XcShell xc_shell;
  std::istringstream is = generateIs(4096);
  std::ostringstream os;

  xc_shell.process(is, os);

  EXPECT_GT(os.str().length(), 4096);
}

std::istringstream generateIs(size_t len) {
  std::stringstream ss;
  for (int i = 0; i < len; i++) {
    ss << "x";
  }
  return std::istringstream(ss.str());
}
