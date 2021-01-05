#include <iostream>
#include <unistd.h>

#include "../common/opcode.h"
#include "fileReader.h"
#include "instructions/instructionFactory.h"
#include "programGenerator.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

bool CheckArguments(int argc, char *argv[]) { return true; }

int main(int argc, char *argv[]) {
  spdlog::info("me88 irCompiler!");

#ifdef NDEBUG
  if (!CheckArguments(argc, argv)) {
    return -1;
  }
  std::string path = argv[1];
#endif

#ifndef NDEBUG
  std::string path = "../irCompiler/debug/";
#endif

  spdlog::info("chdir {}", path);
  chdir(path.c_str());

  std::string compile = "clang -S -emit-llvm *.cpp";
  spdlog::info("{}", compile);
  system(compile.c_str());

  auto const file = "main.ll";

  FileReader reader(file);
  spdlog::info("{}", file);
  std::vector<std::string> instructionStr;

  while (true) {
    std::string line = reader.GetNextLine();

    if (line == "") {
      break;
    }

    instructionStr.push_back(line);
  }

  ProgramGenerator gen(InstructionFactory::MakeInstructions(instructionStr));

  auto const assembly = gen.GetAssembly();

  spdlog::info("Generated Assembly:");
  for (auto const &str : assembly) {
    std::cout << str << std::endl;
  }

  std::ofstream outAsmFile(std::string(file) + ".me88asm");
  for (auto const &code : assembly) {
    outAsmFile << code << std::endl;
  }

  outAsmFile.close();

  auto const binary = gen.GetBinary();
  spdlog::info("Generated Binary:");
  for (auto const &bits : binary) {
    std::cout << bits.to_string() << std::endl;
  }

  std::ofstream outBinFile(std::string(file) + ".me88bin");
  for (auto const &bits : binary) {
    outBinFile << bits << std::endl;
  }

  outBinFile.close();

  return 0;
}