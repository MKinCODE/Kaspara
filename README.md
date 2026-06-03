# Kaspara ♟️

A terminal chess engine written in C++, named after Garry Kasparov.

## Features

- All legal chess moves including castling, en passant, and pawn promotion
- Minimax with alpha-beta pruning at depth 4
- Piece-square table positional evaluation
- Displays depth, nodes, and nodes per second after each engine move
- Human vs engine terminal play
- Clean C++11 codebase that is easy to read and extend

## Build & Run

Build with Make:

```
make
```

Or build directly:

```
g++ -std=c++11 -O2 -o kaspara main.cpp board.cpp engine.cpp
```

Run:

```
./kaspara
```

## How to Play

- Enter moves in UCI format, e.g. `e2e4`
- Use `e7e8q` for promotion to queen (or `r`, `b`, `n`)
- Type `quit` to exit

## How the Engine Works

Kaspara searches ahead with minimax to pick the move that maximizes its advantage. Alpha-beta pruning skips branches that cannot improve the result, speeding up the search. Piece-square tables add a positional bonus or penalty to each piece based on its square, encouraging strong placement on the board.

## Performance

Searches ~394,000 positions per second at depth 4.

## License

MIT
