#include "lexy.h"

//#include "codegenerator.h"
#include "ast.h"
#include "codegenerator.h"
#include "semantic.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "token.h"
#include <fstream>
#include <iostream>
#include <memory>

#include <unistd.h>

int main(int argc, char *argv[]) {
#ifdef NDEBUG
  if (argc == 1) {
    spdlog::error("Missing file name.");
    return 0;
  }
  auto fileName = argv[1];
#endif

#ifndef NDEBUG
  auto debug = true;
  auto fileName = "../src/debugprograms/debug.F7";
#endif
  auto [validToks, tokens] = Lexer::GetTokensFromFile(fileName);

  if (!validToks) {
    return 0;
  }

#ifndef NDEBUG
  Lexer::PrintTokens(tokens);
#endif

  auto [validProg, AST] = CreateAST(tokens);
  if (!validProg) {
    return 0;
  }

  auto [validSemantic, symbols] = AnalyzeSemantic(AST);
  if (!validSemantic) {
    return 0;
  }

  std::ofstream outfile(std::string(fileName) + ".bin");
  auto machinecode = GenerateCode(AST, symbols);

  for (auto code : machinecode) {
    outfile << code << std::endl;
  }

  return 0;
}