#ifndef KASPARA_BOARD_H
#define KASPARA_BOARD_H

#include "types.h"
#include <vector>

class Board {
public:
    Piece board[8][8];
    Color sideToMove;
    int enPassantX;
    int enPassantY;
    bool whiteKingSide;
    bool whiteQueenSide;
    bool blackKingSide;
    bool blackQueenSide;

    // Resets the board to the standard chess starting position.
    void reset();

    // Prints the board state and side to move to the terminal.
    void print() const;

    // Generates all pseudo-legal moves for the side to move.
    std::vector<Move> generatePseudoMoves();

private:
    // Returns true if the coordinate is on the board.
    bool inBounds(int x, int y) const;

    // Adds a move to the move list with optional flags.
    void addMove(std::vector<Move>& moves, int fx, int fy, int tx, int ty,
                 PieceType promo = EMPTY, bool enPassant = false, bool castle = false) const;
};

#endif
