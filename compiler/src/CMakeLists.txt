cmake_minimum_required(VERSION 3.17)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(Compy)

# add the executable
add_executable(
	Compy
	main.cpp
	lexy.cpp
	token.cpp
  ast.cpp
  semantic.cpp
  tree.cpp
  symbolstable.cpp
  codegenerator.cpp
)


#link libs
find_package(spdlog REQUIRED)
target_link_libraries(Compy PRIVATE spdlog::spdlog)