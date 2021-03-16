#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "ast.h"
#include "lexy.h"
#include "parser.h"
#include "semantic.h"
#include "token.h"

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

  auto const ir = Parser::ParseIR(fileName, AST, symbols);
  std::ofstream outirfile(std::string(fileName) + ".ir");
  for (auto const &entry : ir) {
    outirfile << entry << std::endl;
  }
  
  auto const machinecode = Parser::ParseMachineCode(ir);
  std::ofstream outcodefile(std::string(fileName) + ".bin");
  for (auto const &code : machinecode) {
    outcodefile << code << std::endl;
  }

  return 0;
}