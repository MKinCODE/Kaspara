#include "board.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

// Returns the Unicode symbol for a given piece.
static const char* pieceToUnicode(const Piece& piece) {
    if (piece.type == EMPTY) {
        return ".";
    }

    switch (piece.type) {
        case PAWN:
            return piece.color == WHITE ? u8"♙" : u8"♟";
        case KNIGHT:
            return piece.color == WHITE ? u8"♘" : u8"♞";
        case BISHOP:
            return piece.color == WHITE ? u8"♗" : u8"♝";
        case ROOK:
            return piece.color == WHITE ? u8"♖" : u8"♜";
        case QUEEN:
            return piece.color == WHITE ? u8"♕" : u8"♛";
        case KING:
            return piece.color == WHITE ? u8"♔" : u8"♚";
        default:
            return ".";
    }
}

// Resets the board to the standard chess starting position.
void Board::reset() {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            board[y][x] = Piece();
        }
    }

    static const PieceType backRank[8] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };

    for (int x = 0; x < 8; ++x) {
        board[0][x].type = backRank[x];
        board[0][x].color = WHITE;

        board[1][x].type = PAWN;
        board[1][x].color = WHITE;

        board[6][x].type = PAWN;
        board[6][x].color = BLACK;

        board[7][x].type = backRank[x];
        board[7][x].color = BLACK;
    }

    sideToMove = WHITE;
}

// Prints the board state and side to move to the terminal.
void Board::print() const {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "  a b c d e f g h\n";
    for (int y = 7; y >= 0; --y) {
        std::cout << (y + 1) << " ";
        for (int x = 0; x < 8; ++x) {
            std::cout << pieceToUnicode(board[y][x]) << " ";
        }
        std::cout << (y + 1) << "\n";
    }
    std::cout << "  a b c d e f g h\n";

    const char* side =
        (sideToMove == WHITE) ? "White" : (sideToMove == BLACK) ? "Black" : "None";
    std::cout << "Side to move: " << side << "\n";
}
