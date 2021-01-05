cmake_minimum_required(VERSION 3.17)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----- irCompiler 
project(me88irCompiler)

# add the executable
add_executable(
  me88irCompiler
  ./irCompiler/main.cpp
  ./common/opcode.cpp
  ./irCompiler/fileReader.cpp
  ./irCompiler/instructions/instruction.cpp
  ./irCompiler/instructions/instructionFactory.cpp
  ./irCompiler/instructions/alloca.cpp
  ./irCompiler/instructions/store.cpp
  ./irCompiler/instructions/load.cpp
  ./irCompiler/instructions/functionDefinition.cpp
  ./irCompiler/stackTracker.cpp
  ./irCompiler/variableTable.cpp
  ./irCompiler/programGenerator.cpp
)

# add tests executable 
add_executable(
  me88irCompilerTests 
  ./irCompiler/tests/mainTests.cpp
  ./common/opcode.cpp
  ./irCompiler/symbolTable.cpp
  ./irCompiler/fileReader.cpp
  ./irCompiler/instructions/instruction.cpp
  ./irCompiler/instructions/instructionFactory.cpp
  ./irCompiler/instructions/alloca.cpp
  ./irCompiler/instructions/store.cpp
  ./irCompiler/instructions/load.cpp
  ./irCompiler/instructions/functionDefinition.cpp
  ./irCompiler/stackTracker.cpp
  ./irCompiler/variableTable.cpp  
  ./irCompiler/programGenerator.cpp
) 

#link libs
find_package(spdlog REQUIRED)
target_link_libraries(me88irCompiler PRIVATE spdlog::spdlog)

target_link_libraries(me88irCompilerTests PRIVATE gtest)
target_link_libraries(me88irCompilerTests PRIVATE spdlog::spdlog)

# ----- End irCompiler

# ----- Processor
project(ME88)

# add the executable
add_executable(
	ME88 
	./pc/src/main.cpp	
	./pc/src/microPC.cpp
	./pc/src/processor.cpp
	./pc/src/bus.cpp
	./pc/src/memdevice.cpp
	./pc/src/instruction.cpp
	./pc/src/printer.cpp
)

#link libs
target_link_libraries(ME88 ncurses)
