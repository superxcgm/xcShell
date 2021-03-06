#include "xcshell/build_in/alias.h"

#include <gtest/gtest.h>

TEST(AliasTest, Execute_ShouldPrintEmptyLineForNoAlias) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;

  auto rtn = alias.Execute(no_args, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");
}

TEST(AliasTest, Execute_ShouldPrintSingleAlias) {
  Alias alias;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {"ls='ls -G'"};

  auto rtn = alias.Execute(create_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  std::vector<std::string> list_single_alias = {"ls"};
  rtn = alias.Execute(list_single_alias, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "ls='ls -G'\n");
}

TEST(AliasTest, Execute_ShouldPrintAlias) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {"ls='ls -G'"};

  auto rtn = alias.Execute(create_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  std::vector<std::string> create_another_alias = {"ll='ls -lh'"};

  rtn = alias.Execute(create_another_alias, os, os_err);
  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "");

  rtn = alias.Execute(no_args, os, os_err);

  EXPECT_EQ(rtn, 0);
  EXPECT_EQ(os.str(), "ll='ls -lh'\nls='ls -G'\n");
}

TEST(AliasTest, Replace_ShouldRepleaseIfAliasFound) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {"ls='ls -G'"};

  alias.Execute(create_alias, os, os_err);
  EXPECT_EQ(alias.Replace("ls"), "ls -G");
}

TEST(AliasTest, Replace_ShouldRepleaseMultiTimesIfAliasFound) {
  Alias alias;
  std::vector<std::string> no_args;
  std::ostringstream os;
  std::ostringstream os_err;
  std::vector<std::string> create_alias = {"ls='ls -G'"};

  alias.Execute(create_alias, os, os_err);
  create_alias = {"ll='ls -lh'"};
  alias.Execute(create_alias, os, os_err);

  EXPECT_EQ(alias.Replace("ll"), "ls -G -lh");
}

TEST(AliasTest, Replace_ShouldNotRepleaseIfAliasNotFound) {
  Alias alias;
  EXPECT_EQ(alias.Replace("ls"), "ls");
}
