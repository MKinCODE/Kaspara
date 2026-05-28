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

// Applies a move to the board and returns a record for undo.
Board::MoveRecord Board::makeMove(const Move& m) {
    MoveRecord record;
    record.move = m;
    record.captured = Piece();
    record.moved = board[m.fromY][m.fromX];
    record.whiteKingSide = whiteKingSide;
    record.whiteQueenSide = whiteQueenSide;
    record.blackKingSide = blackKingSide;
    record.blackQueenSide = blackQueenSide;
    record.enPassantX = enPassantX;
    record.enPassantY = enPassantY;
    record.sideToMove = sideToMove;

    const int fx = m.fromX;
    const int fy = m.fromY;
    const int tx = m.toX;
    const int ty = m.toY;
    const Piece movedPiece = record.moved;

    enPassantX = -1;
    enPassantY = -1;

    if (m.isEnPassant && movedPiece.type == PAWN) {
        const int dir = (movedPiece.color == WHITE) ? 1 : -1;
        const int capY = ty - dir;
        record.captured = board[capY][tx];
        board[capY][tx] = Piece();
    } else {
        record.captured = board[ty][tx];
    }

    board[fy][fx] = Piece();
    board[ty][tx] = movedPiece;

    if (movedPiece.type == PAWN && m.promotion != EMPTY) {
        board[ty][tx].type = m.promotion;
        board[ty][tx].color = movedPiece.color;
    }

    if (m.isCastle && movedPiece.type == KING) {
        if (tx == 6) {
            board[ty][5] = board[ty][7];
            board[ty][7] = Piece();
        } else if (tx == 2) {
            board[ty][3] = board[ty][0];
            board[ty][0] = Piece();
        }
    }

    if (movedPiece.type == KING) {
        if (movedPiece.color == WHITE) {
            whiteKingSide = false;
            whiteQueenSide = false;
        } else {
            blackKingSide = false;
            blackQueenSide = false;
        }
    }

    if (movedPiece.type == ROOK) {
        if (movedPiece.color == WHITE) {
            if (fx == 0 && fy == 0) {
                whiteQueenSide = false;
            } else if (fx == 7 && fy == 0) {
                whiteKingSide = false;
            }
        } else {
            if (fx == 0 && fy == 7) {
                blackQueenSide = false;
            } else if (fx == 7 && fy == 7) {
                blackKingSide = false;
            }
        }
    }

    if (movedPiece.type == PAWN) {
        const int diff = ty - fy;
        if (diff == 2 || diff == -2) {
            const int dir = (movedPiece.color == WHITE) ? 1 : -1;
            enPassantX = fx;
            enPassantY = fy + dir;
        }
    }

    sideToMove = (sideToMove == WHITE) ? BLACK : WHITE;
    return record;
}

// Restores the board to the state before a move was made.
void Board::undoMove(const MoveRecord& record) {
    const Move& m = record.move;
    const int fx = m.fromX;
    const int fy = m.fromY;
    const int tx = m.toX;
    const int ty = m.toY;
    const Piece movedPiece = record.moved;

    sideToMove = record.sideToMove;
    whiteKingSide = record.whiteKingSide;
    whiteQueenSide = record.whiteQueenSide;
    blackKingSide = record.blackKingSide;
    blackQueenSide = record.blackQueenSide;
    enPassantX = record.enPassantX;
    enPassantY = record.enPassantY;

    board[fy][fx] = movedPiece;

    if (m.isEnPassant && movedPiece.type == PAWN) {
        board[ty][tx] = Piece();
        const int dir = (movedPiece.color == WHITE) ? 1 : -1;
        const int capY = ty - dir;
        board[capY][tx] = record.captured;
    } else {
        board[ty][tx] = record.captured;
    }

    if (m.isCastle && movedPiece.type == KING) {
        if (tx == 6) {
            board[ty][7] = board[ty][5];
            board[ty][5] = Piece();
        } else if (tx == 2) {
            board[ty][0] = board[ty][3];
            board[ty][3] = Piece();
        }
    }
}

// Returns true if a square is attacked by the given color.
bool Board::isSquareAttacked(int x, int y, Color byColor) const {
    const int pawnDir = (byColor == WHITE) ? -1 : 1;
    const int pawnY = y + pawnDir;
    if (inBounds(x - 1, pawnY)) {
        const Piece& p = board[pawnY][x - 1];
        if (p.type == PAWN && p.color == byColor) {
            return true;
        }
    }
    if (inBounds(x + 1, pawnY)) {
        const Piece& p = board[pawnY][x + 1];
        if (p.type == PAWN && p.color == byColor) {
            return true;
        }
    }

    static const int knightOffsets[8][2] = {
        { 1, 2 }, { 2, 1 }, { 2, -1 }, { 1, -2 },
        { -1, -2 }, { -2, -1 }, { -2, 1 }, { -1, 2 }
    };
    for (int i = 0; i < 8; ++i) {
        const int nx = x + knightOffsets[i][0];
        const int ny = y + knightOffsets[i][1];
        if (!inBounds(nx, ny)) {
            continue;
        }
        const Piece& p = board[ny][nx];
        if (p.type == KNIGHT && p.color == byColor) {
            return true;
        }
    }

    static const int diagDirs[4][2] = {
        { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }
    };
    for (int i = 0; i < 4; ++i) {
        int tx = x + diagDirs[i][0];
        int ty = y + diagDirs[i][1];
        while (inBounds(tx, ty)) {
            const Piece& p = board[ty][tx];
            if (p.type != EMPTY) {
                if (p.color == byColor && (p.type == BISHOP || p.type == QUEEN)) {
                    return true;
                }
                break;
            }
            tx += diagDirs[i][0];
            ty += diagDirs[i][1];
        }
    }

    static const int straightDirs[4][2] = {
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
    };
    for (int i = 0; i < 4; ++i) {
        int tx = x + straightDirs[i][0];
        int ty = y + straightDirs[i][1];
        while (inBounds(tx, ty)) {
            const Piece& p = board[ty][tx];
            if (p.type != EMPTY) {
                if (p.color == byColor && (p.type == ROOK || p.type == QUEEN)) {
                    return true;
                }
                break;
            }
            tx += straightDirs[i][0];
            ty += straightDirs[i][1];
        }
    }

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
            const Piece& p = board[ty][tx];
            if (p.type == KING && p.color == byColor) {
                return true;
            }
        }
    }

    return false;
}

// Returns true if the specified side's king is in check.
bool Board::inCheck(Color color) const {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Piece& p = board[y][x];
            if (p.type == KING && p.color == color) {
                const Color attacker = (color == WHITE) ? BLACK : WHITE;
                return isSquareAttacked(x, y, attacker);
            }
        }
    }
    return false;
}

// Generates all legal moves for the side to move.
std::vector<Move> Board::generateLegalMoves() {
    std::vector<Move> legalMoves;
    const std::vector<Move> pseudoMoves = generatePseudoMoves();
    for (size_t i = 0; i < pseudoMoves.size(); ++i) {
        const Move& m = pseudoMoves[i];
        MoveRecord record = makeMove(m);
        const bool kingInCheck = inCheck(record.sideToMove);
        undoMove(record);
        if (!kingInCheck) {
            legalMoves.push_back(m);
        }
    }
    return legalMoves;
}
