# Imperative and Functional Games

![c++ and haskell video game logo](/shared/background.png)

## About

Several simple games built with SDL in both C++ and Haskell.

The C++ versions of the games are based on the tutorials at [www.aaroncox.net/tutorials/arcade](http://www.aaroncox.net/tutorials/arcade) with a few updates made. First, everything was upgraded to SDL2 and C++14. Some refactoring was done to reduce duplicate code. Some constants were moved from separate `.h` files to simplify things. A basic Makefile was introduced.

The Haskell versions are built second, translating fairly directly from the C++ versions. For example, the Haskell version of "Intro" is built with the following `Game` monad replacing `void` from the C++ version. This is how we sneak in state and IO, and can program in a way that feels imperative.

```haskell
type Game = StateT GameState IO

data GameState = GameState
  { window :: Window
  , renderer :: Renderer
  , timer :: Word32
  , bitmap :: Surface
  , statestack :: [Game ()]
  }
```

## Usage

C++: compile with `make`, then run `./Game`

Haskell: compile with `ghc Game.hs`, then run `./Game`
