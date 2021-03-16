cmake_minimum_required(VERSION 3.17)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----- Compiler 
project(me88Compiler)

# add the executable
add_executable(
  me88Compiler
  ./common/opcode.cpp
  ./compiler/src/main.cpp
  ./compiler/src/lexy.cpp
  ./compiler/src/token.cpp
  ./compiler/src/ast.cpp
  ./compiler/src/semantic.cpp
  ./compiler/src/tree.cpp
  ./compiler/src/symbolstable.cpp
  ./compiler/src/symbolstable.cpp
  ./compiler/src/parser.cpp  
)

#add tests executable 
add_executable(
  me88CompilerTests 
  ./common/opcode.cpp
  ./compiler/src/tests/mainTest.cpp 
  ./compiler/src/lexy.cpp
  ./compiler/src/token.cpp
  ./compiler/src/tree.cpp
  ./compiler/src/ast.cpp
  ./compiler/src/semantic.cpp
  ./compiler/src/symbolstable.cpp
  ./compiler/src/parser.cpp
) 

#link libs
find_package(spdlog REQUIRED)
target_link_libraries(me88Compiler PRIVATE spdlog::spdlog)

target_link_libraries(me88CompilerTests PRIVATE gtest)
target_link_libraries(me88CompilerTests PRIVATE spdlog::spdlog)

# ----- End compiler

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
