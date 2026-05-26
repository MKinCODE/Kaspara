#ifndef KASPARA_BOARD_H
#define KASPARA_BOARD_H

#include "types.h"

class Board {
public:
    Piece board[8][8];
    Color sideToMove;

    // Resets the board to the standard chess starting position.
    void reset();

    // Prints the board state and side to move to the terminal.
    void print() const;
};

#endif
