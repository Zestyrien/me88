# ME88 Compiler

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This is the compiler for the ME88. There are a couple of examples for programs in the programs directory (.F7 extension).
The language supported features
	* unsigned 8 bits variables 
	* if else statements
	* while loops
	* scopes
	* additions and subtractions
  * comments

# Installation 

* create a build directory 
* run: cmake 'path_to_compiler_src' 
* run: make

# Usage

./Compy 'path_to_your_F7_program/program.F7'

The output will be 'path_to_your_F7_program/program.F7.bin'.

Use -d -D to print the output.

# TO DO in no particular order

* <= >= 
* functions
* 8 bit integers
* booleans ( maybe )
* multiplication and division ( maybe )
* precedence for the operators in expression 
* parentheses in expressions
* implement an intermidiate language in order to add some optimization
* add assembly support in order to write some primitives

# Example program with all the features
```
# '#' is for the comments

u8 x; 
u8 y := 5;

if y > 4 then
	u8 z := 3;
	x := y - z;
else
	x := y + 10;
endif

while y > 0 loop
	x := x + 1;
	y := y - 1;
endloop

```