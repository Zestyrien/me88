# ME88

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This is direclty translated from Verilog and it is not meant to execute code as fast as possible.  

# Installation 

* create a build directory 
* run: cmake 'path_to_compiler_src' 
* run: make
* ./ME88 'path_to_your_programs_folder'

# TO DO in no particular order

* interrupts with and interrupts controller
* load interrupts table
* add a keyboard

# Usage

You can use the "-d" argument so the processor will stop after every clock cycle and wait for enter.
The progrmam will run "../../programs/eprom.F7.bin". It is hardcoded for now, I will fix this at some point.
