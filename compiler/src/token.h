#pragma once

#include <string>

enum class TokenType
{
  Type = 0,
  Variable,
  Operator,
  Assign,
  Number,
  Semicol,
  If,
  Then,
  Else,
  EndIf,
  While,
  Column,
  Comma,
  OpenBracket,
  CloseBracket,
  OpenCurly,
  CloseCurly,
  Loop,
  EndLoop,
  Unknown
};

class Token
{
public:
  Token() = delete;
  Token(const std::string &str, int line);
  static bool IsToken(const std::string &str);
  static TokenType FindType(const std::string &str);
  std::string GetValue() const;
  TokenType GetType() const;
  int GetLine() const;
  void Print() const;

private:
  TokenType m_type;
  std::string m_value;
  int m_line;
};