#ifndef KASPARA_BOARD_H
#define KASPARA_BOARD_H

#include "types.h"
#include <vector>

class Board {
public:
    struct MoveRecord {
        Move move;
        Piece captured;
        Piece moved;
        bool whiteKingSide;
        bool whiteQueenSide;
        bool blackKingSide;
        bool blackQueenSide;
        int enPassantX;
        int enPassantY;
        Color sideToMove;
    };

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

    // Applies a move to the board and returns a record for undo.
    MoveRecord makeMove(const Move& m);

    // Restores the board to the state before a move was made.
    void undoMove(const MoveRecord& record);

    // Returns true if a square is attacked by the given color.
    bool isSquareAttacked(int x, int y, Color byColor) const;

    // Returns true if the specified side's king is in check.
    bool inCheck(Color color) const;

    // Generates all legal moves for the side to move.
    std::vector<Move> generateLegalMoves();

private:
    // Returns true if the coordinate is on the board.
    bool inBounds(int x, int y) const;

    // Adds a move to the move list with optional flags.
    void addMove(std::vector<Move>& moves, int fx, int fy, int tx, int ty,
                 PieceType promo = EMPTY, bool enPassant = false, bool castle = false) const;
};

#endif
