#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "lexy.h"
#include "ast.h"
#include "semantic.h"
#include "token.h"
#include "parser.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

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
  auto fileName = "../compiler/src/debugprograms/debug.F7";
#endif
  auto [validToks, tokens] = Lexer::GetTokensFromFile(fileName);

  if (!validToks) {
    return 0;
  }

#ifndef NDEBUG
  Lexer::PrintTokens(tokens);
#endif

  auto const [validProg, AST] = CreateAST(tokens);
  if (!validProg) {
    return 0;
  }

  auto const [validSemantic, symbols] = AnalyzeSemantic(AST);
  if (!validSemantic) {
    return 0;
  }

/*
  auto machinecode = GenerateCode(AST, symbols);

  std::ofstream outfile(std::string(fileName) + ".bin");
  for (auto code : machinecode) {
    outfile << code << std::endl;
  }
*/

  auto const ir = Parser::ParseIR(AST, symbols);

  return 0;
}