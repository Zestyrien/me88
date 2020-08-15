#include "token.h"
#include "unordered_map"
#include <algorithm>
#include <iostream>

Token::Token(const std::string &str, int line) : m_value(str), m_line(line) {
  m_type = FindType(str);
}

bool Token::IsToken(const std::string &str) {
  return FindType(str) != TokenType::Unknown;
}

bool StartsWithLetter(const std::string &str) { return isalpha(str[0]); }

bool IsAlphaNumeric(const std::string &str) {
  return std::find_if(str.begin(), str.end(), [](char c) {
           return !(isalnum(c) || (c == ' '));
         }) == str.end();
}

bool IsOnlyNumbers(const std::string &str) {
  return std::find_if(str.begin(), str.end(), [](char c) {
           return !(isdigit(c) || (c == ' '));
         }) == str.end();
}

TokenType Token::FindType(const std::string &str) {
  if (str == "u8" || str == "void")
    return TokenType::Type;
  else if (str == ":=")
    return TokenType::Assign;
  else if (str == "+" || str == "-" || str == "<<" || str == ">>" ||
           str == ">" || str == "<" || str == ">=" || str == "<=" ||
           str == "==" || str == "!=")
    return TokenType::Operator;
  else if (str == ";")
    return TokenType::Semicol;
  else if (str == ":")
    return TokenType::Column;
  else if (str == ",")
    return TokenType::Comma;
  else if (str == "(")
    return TokenType::OpenBracket;
  else if (str == ")")
    return TokenType::CloseBracket;
  else if (str == "{")
    return TokenType::OpenCurly;
  else if (str == "}")
    return TokenType::CloseCurly;
  else if (str == "if")
    return TokenType::If;
  else if (str == "then")
    return TokenType::Then;
  else if (str == "else")
    return TokenType::Else;
  else if (str == "endif")
    return TokenType::EndIf;
  else if (str == "while")
    return TokenType::While;
  else if (str == "loop")
    return TokenType::Loop;
  else if (str == "endloop")
    return TokenType::EndLoop;
  else if (StartsWithLetter(str) && IsAlphaNumeric(str))
    return TokenType::Variable;
  else if (IsOnlyNumbers(str))
    return TokenType::Number;
  else
    return TokenType::Unknown;
}

std::string Token::GetValue() const { return m_value; }

TokenType Token::GetType() const { return m_type; }

int Token::GetLine() const { return m_line; }

void Token::Print() const {
  std::cout << "Type: [" << (int)m_type << "]";
  std::cout << " Value: [" << m_value << "]";
  std::cout << " Line: [" << m_line << "]";
}