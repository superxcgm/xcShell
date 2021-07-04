#include <gtest/gtest.h>

#include "../../include/xcshell/build_in/alias.h"

TEST(AliasTest, ShouldPrintEmptyLineForNoAlias) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;

  auto rtn = alias.Execute(no_args, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");
}

TEST(AliasTest, ShouldPrintSingleAlias) {
  Alias alias;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {
      "ls='ls -G'"
  };

  auto rtn =  alias.Execute(create_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  std::vector<std::string> list_single_alias = {
      "ls"
  };
  rtn = alias.Execute(list_single_alias, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "ls='ls -G'\n");
}

TEST(AliasTest, ShouldPrintAlias) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {
      "ls='ls -G'"
  };

  auto rtn =  alias.Execute(create_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  std::vector<std::string> create_another_alias = {
      "ll='ls -lh'"
  };

  rtn =  alias.Execute(create_another_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  rtn = alias.Execute(no_args, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "ll='ls -lh'\nls='ls -G'\n");
}