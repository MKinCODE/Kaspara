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
    enPassantX = -1;
    enPassantY = -1;
    whiteKingSide = true;
    whiteQueenSide = true;
    blackKingSide = true;
    blackQueenSide = true;
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

// Returns true if the coordinate is on the board.
bool Board::inBounds(int x, int y) const {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

// Adds a move to the move list with optional flags.
void Board::addMove(std::vector<Move>& moves, int fx, int fy, int tx, int ty,
                    PieceType promo, bool enPassant, bool castle) const {
    Move move;
    move.fromX = fx;
    move.fromY = fy;
    move.toX = tx;
    move.toY = ty;
    move.promotion = promo;
    move.isEnPassant = enPassant;
    move.isCastle = castle;
    moves.push_back(move);
}

// Generates all pseudo-legal moves for the side to move.
std::vector<Move> Board::generatePseudoMoves() {
    std::vector<Move> moves;
    const int dir = (sideToMove == WHITE) ? 1 : -1;
    const int startRank = (sideToMove == WHITE) ? 1 : 6;
    const int promotionRank = (sideToMove == WHITE) ? 7 : 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Piece& piece = board[y][x];
            if (piece.color != sideToMove) {
                continue;
            }

            switch (piece.type) {
                case PAWN: {
                    const int oneStepY = y + dir;
                    if (inBounds(x, oneStepY) && board[oneStepY][x].type == EMPTY) {
                        if (oneStepY == promotionRank) {
                            addMove(moves, x, y, x, oneStepY, QUEEN);
                            addMove(moves, x, y, x, oneStepY, ROOK);
                            addMove(moves, x, y, x, oneStepY, BISHOP);
                            addMove(moves, x, y, x, oneStepY, KNIGHT);
                        } else {
                            addMove(moves, x, y, x, oneStepY);
                            if (y == startRank) {
                                const int twoStepY = y + 2 * dir;
                                if (inBounds(x, twoStepY) && board[twoStepY][x].type == EMPTY) {
                                    addMove(moves, x, y, x, twoStepY);
                                }
                            }
                        }
                    }

                    for (int dx = -1; dx <= 1; dx += 2) {
                        const int cx = x + dx;
                        const int cy = y + dir;
                        if (!inBounds(cx, cy)) {
                            continue;
                        }

                        if (board[cy][cx].type != EMPTY && board[cy][cx].color != sideToMove) {
                            if (cy == promotionRank) {
                                addMove(moves, x, y, cx, cy, QUEEN);
                                addMove(moves, x, y, cx, cy, ROOK);
                                addMove(moves, x, y, cx, cy, BISHOP);
                                addMove(moves, x, y, cx, cy, KNIGHT);
                            } else {
                                addMove(moves, x, y, cx, cy);
                            }
                        }

                        if (cx == enPassantX && cy == enPassantY) {
                            addMove(moves, x, y, cx, cy, EMPTY, true, false);
                        }
                    }
                    break;
                }
                case KNIGHT: {
                    static const int offsets[8][2] = {
                        { 1, 2 }, { 2, 1 }, { 2, -1 }, { 1, -2 },
                        { -1, -2 }, { -2, -1 }, { -2, 1 }, { -1, 2 }
                    };
                    for (int i = 0; i < 8; ++i) {
                        const int tx = x + offsets[i][0];
                        const int ty = y + offsets[i][1];
                        if (!inBounds(tx, ty)) {
                            continue;
                        }
                        if (board[ty][tx].type == EMPTY || board[ty][tx].color != sideToMove) {
                            addMove(moves, x, y, tx, ty);
                        }
                    }
                    break;
                }
                case BISHOP:
                case ROOK:
                case QUEEN: {
                    static const int bishopDirs[4][2] = {
                        { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }
                    };
                    static const int rookDirs[4][2] = {
                        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
                    };

                    const int (*dirs)[2] = nullptr;
                    int dirCount = 0;
                    int combinedDirs[8][2];

                    if (piece.type == BISHOP) {
                        dirs = bishopDirs;
                        dirCount = 4;
                    } else if (piece.type == ROOK) {
                        dirs = rookDirs;
                        dirCount = 4;
                    } else {
                        for (int i = 0; i < 4; ++i) {
                            combinedDirs[i][0] = bishopDirs[i][0];
                            combinedDirs[i][1] = bishopDirs[i][1];
                        }
                        for (int i = 0; i < 4; ++i) {
                            combinedDirs[i + 4][0] = rookDirs[i][0];
                            combinedDirs[i + 4][1] = rookDirs[i][1];
                        }
                        dirs = combinedDirs;
                        dirCount = 8;
                    }

                    for (int i = 0; i < dirCount; ++i) {
                        int tx = x + dirs[i][0];
                        int ty = y + dirs[i][1];
                        while (inBounds(tx, ty)) {
                            if (board[ty][tx].type == EMPTY) {
                                addMove(moves, x, y, tx, ty);
                            } else {
                                if (board[ty][tx].color != sideToMove) {
                                    addMove(moves, x, y, tx, ty);
                                }
                                break;
                            }
                            tx += dirs[i][0];
                            ty += dirs[i][1];
                        }
                    }
                    break;
                }
                case KING: {
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) {
                                continue;
                            }
                            const int tx = x + dx;
                            const int ty = y + dy;
                            if (!inBounds(tx, ty)) {
                                continue;
                            }
                            if (board[ty][tx].type == EMPTY || board[ty][tx].color != sideToMove) {
                                addMove(moves, x, y, tx, ty);
                            }
                        }
                    }

                    if (sideToMove == WHITE && x == 4 && y == 0) {
                        if (whiteKingSide && board[0][5].type == EMPTY && board[0][6].type == EMPTY &&
                            board[0][7].type == ROOK && board[0][7].color == WHITE) {
                            addMove(moves, x, y, 6, 0, EMPTY, false, true);
                        }
                        if (whiteQueenSide && board[0][1].type == EMPTY && board[0][2].type == EMPTY &&
                            board[0][3].type == EMPTY && board[0][0].type == ROOK &&
                            board[0][0].color == WHITE) {
                            addMove(moves, x, y, 2, 0, EMPTY, false, true);
                        }
                    } else if (sideToMove == BLACK && x == 4 && y == 7) {
                        if (blackKingSide && board[7][5].type == EMPTY && board[7][6].type == EMPTY &&
                            board[7][7].type == ROOK && board[7][7].color == BLACK) {
                            addMove(moves, x, y, 6, 7, EMPTY, false, true);
                        }
                        if (blackQueenSide && board[7][1].type == EMPTY && board[7][2].type == EMPTY &&
                            board[7][3].type == EMPTY && board[7][0].type == ROOK &&
                            board[7][0].color == BLACK) {
                            addMove(moves, x, y, 2, 7, EMPTY, false, true);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    return moves;
}
