How to run:

CLion IDE ->
  clone the repository and use the included CMakelist's target when you click run.

Command Line ->
  build the CMakelist with main.cpp in the same folder.
  OR
  compile main.cpp yourself using the compiler of your choice, then run the following executable.


About:

Generates a NxN dungeon with predefined room adjacency rules and write the last generated dungeon into an output file "output.txt".

Program arguments:
  arg 1 -> the amount of iterations to runt he dungeon generator (for testing purposes). only the last dungeon is written to file.
  arg 2 -> defines the NxN size, where N is the value of arg2.
