#include "lexy.h"

#include <iostream>
#include <fstream>
#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "token.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		spdlog::error("Missing file name.");
		return 0;
	}

	bool debug = false;
	if (argc > 2)
	{
		debug = argv[2] == std::string("-d") || argv[2] == std::string("-D");
	}

	auto [validToks, tokens] = Lexer::GetTokensFromFile(argv[1]);

	if (!validToks)
	{
		return 0;
	}

	if (debug)
	{
		Lexer::PrintTokens(tokens);
	}

	auto [validProg, program] = Parser::ParseProgram(tokens);
	if (!validProg)
	{
		return 0;
	}

	if (debug)
	{
		Parser::PrintTree(program);
	}

	auto [validSymbols, symbols] = Parser::GenerateSymbolTable(program);
	if (!validSymbols)
	{
		return 0;
	}

	if (debug)
	{
		symbols.Print();
	}

	std::ofstream outfile(std::string(argv[1]) + ".bin");
	auto machinecode = CodeGen::GenerateCode(program, symbols);
	
	if (debug)
	{
		CodeGen::Print(machinecode);
	}
	
	for (auto code : machinecode)
	{
		outfile << code << std::endl;
	}

	return 0;
}