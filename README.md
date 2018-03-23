# Imperative and Functional Games

Several simple games built with SDL in both C++ and Haskell.

The games are based on the tutorials at [www.aaroncox.net/tutorials/arcade](http://www.aaroncox.net/tutorials/arcade) with a few updates made. First, everything was upgraded to SDL2 and C++14. Some refactoring was done to reduce duplicate code. Finally, constants were moved from a separate `Defines.h` file to the top of relevant files, and a simple Makefile was introduced.

## Usage

C++: compile with `make`, then run `./Game`

Haskell: compile with `ghc Game.hs`, then run './game'

