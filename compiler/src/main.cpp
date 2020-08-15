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

int main(int argc, char *argv[]) {
  if (argc == 1) {
    spdlog::error("Missing file name.");
    return 0;
  }

  bool debug = false;
  if (argc > 2) {
    debug = argv[2] == std::string("-d") || argv[2] == std::string("-D");
  }

  auto [validToks, tokens] = Lexer::GetTokensFromFile(argv[1]);

  if (!validToks) {
    return 0;
  }

  if (debug) {
    Lexer::PrintTokens(tokens);
  }

  auto [validProg, AST] = CreateAST(tokens, debug);
  if (!validProg) {
    return 0;
  }

  auto [validSemantic, symbols] = AnalyzeSemantic(AST, debug);
  if (!validSemantic) {
    return 0;
  }

  std::ofstream outfile(std::string(argv[1]) + ".bin");
  auto machinecode = GenerateCode(AST, symbols, debug);

  for (auto code : machinecode) {
    outfile << code << std::endl;
  }

  return 0;
}