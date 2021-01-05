#include <gtest/gtest.h>

#include "../symbolTable.h"

TEST(SymbolTableClass, GetSymbolWhenSymbolIsNotPresent) {
  SymbolTable table;

  auto const symbol = table.GetSymbol("non present");

  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 0);
  EXPECT_TRUE(symbol.name == "");
}

TEST(SymbolTableClass, GetSymbolAndAddSymbolWhenSymbolsArePresent) {
  SymbolTable table;
  std::string const symbol1 = "a symbol";
  std::string const symbol2 = "another symbol";
  std::string const symbol3 = "yet another symbol";

  table.AddSymbol(symbol1);
  table.AddSymbol(symbol2);
  table.AddSymbol(symbol3);

  auto symbol = table.GetSymbol(symbol1);
  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 0);
  EXPECT_TRUE(symbol.name == "a symbol");

  symbol = table.GetSymbol(symbol2);
  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 1);
  EXPECT_TRUE(symbol.name == "another symbol");

  symbol = table.GetSymbol(symbol3);
  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 2);
  EXPECT_TRUE(symbol.name == "yet another symbol");
}

TEST(SymbolTableClass, RemoveSymbolWhenSymbolsArePresent) {
  SymbolTable table;
  std::string const symbol1 = "a symbol";

  table.AddSymbol(symbol1);

  auto symbol = table.GetSymbol(symbol1);
  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 0);
  EXPECT_TRUE(symbol.name == "a symbol");

  table.RemoveSymbol("a symbol");

  symbol = table.GetSymbol(symbol1);
  EXPECT_TRUE(symbol.ss == 0);
  EXPECT_TRUE(symbol.sp == 0);
  EXPECT_TRUE(symbol.name == "");
}