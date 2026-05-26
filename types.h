#ifndef KASPARA_TYPES_H
#define KASPARA_TYPES_H

enum Color { WHITE, BLACK, NONE };
enum PieceType { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

struct Piece {
    PieceType type;
    Color color;
    // Default constructor: creates an empty piece with no color.
    Piece() : type(EMPTY), color(NONE) {}
};

class Move {
public:
    int fromX;
    int fromY;
    int toX;
    int toY;
    PieceType promotion;
    bool isEnPassant;
    bool isCastle;
    // Default constructor: initializes a neutral move with no flags.
    Move()
        : fromX(0),
          fromY(0),
          toX(0),
          toY(0),
          promotion(EMPTY),
          isEnPassant(false),
          isCastle(false) {}
};

#endif
