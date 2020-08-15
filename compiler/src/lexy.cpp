#include "lexy.h"

#include "token.h"
#include <algorithm>
#include <fstream>
#include <iostream>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

std::tuple<bool, std::vector<Token>>
Lexer::GetTokensFromFile(const std::string &filename) {
  std::vector<Token> tokens;
  std::ifstream file;
  file.open(filename);
  if (!file.is_open()) {
    spdlog::error("Can't open file.");
    return {false, tokens};
  };

  char c;
  std::string tok;
  std::string lastTok;
  int line = 1;
  while (file.get(c)) {
    bool isendl = c == '\n';
    if (isspace(c) || isendl) {
      if (isendl) {
        line++;
      }

      if (tok != "" && !Token::IsToken(tok) && !Token::IsToken(lastTok + tok)) {
        spdlog::error("Syntax error, '{}' not recognised at line {}", tok,
                      line);
        return {false, tokens};
      } else {
        lastTok = "";
        continue;
      }
    } else {
      tok += c;
    }

    bool add = false;
    if (tokens.size() > 0 && lastTok != "" && Token::IsToken(lastTok + tok)) {
      tokens.pop_back();
      tok = lastTok + tok;
      add = true;
    } else if (Token::IsToken(tok)) {
      add = true;
    }

    if (add) {
      tokens.emplace_back(tok, line);
      lastTok = tok.c_str();
      tok = "";
    }
  }

  return {true, tokens};
}

void Lexer::PrintTokens(const std::vector<Token> &tokens) {
  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
            << std::endl;
  std::cout << "-----------------------TOKENS-----------------------"
            << std::endl;

  for (const auto &tok : tokens) {
    std::cout << "{ ";
    tok.Print();
    std::cout << " }" << std::endl;
  }
  std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
            << std::endl;
}