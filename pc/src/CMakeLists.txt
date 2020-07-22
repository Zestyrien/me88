
cmake_minimum_required(VERSION 3.17)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(ME88)

# add the executable
add_executable(
	ME88 
	main.cpp	
	microPC.cpp
	processor.cpp
	bus.cpp
	memdevice.cpp
	instruction.cpp
	printer.cpp
)

#link libs
target_link_libraries(ME88 ncurses)

