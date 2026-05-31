#include "engine.h"

#include <chrono>
#include <limits>

// Creates an engine with a maximum search depth.
Engine::Engine(int depth) : maxDepth(depth), nodes(0), lastNodes(0), lastNps(0.0) {}

// Finds the best move for the current board position.
Move Engine::findBestMove(Board& board) {
    nodes = 0;
    const auto start = std::chrono::high_resolution_clock::now();

    const std::vector<Move> moves = board.generateLegalMoves();
    Move bestMove;
    int bestScore = std::numeric_limits<int>::min();
    int alpha = -1000000;
    const int beta = 1000000;
    const Color side = board.sideToMove;
    const Color nextSide = (side == WHITE) ? BLACK : WHITE;

    for (size_t i = 0; i < moves.size(); ++i) {
        const Move& m = moves[i];
        Board::MoveRecord record = board.makeMove(m);
        const int score = -negamax(board, maxDepth - 1, -beta, -alpha, nextSide);
        board.undoMove(record);

        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed = end - start;
    lastNodes = nodes;
    lastNps = (elapsed.count() > 0.0) ? (static_cast<double>(nodes) / elapsed.count()) : 0.0;
    return bestMove;
}

// Returns the depth used for the last search.
int Engine::lastDepth() const {
    return maxDepth;
}

// Returns the number of nodes evaluated in the last search.
long long Engine::lastNodesEvaluated() const {
    return lastNodes;
}

// Returns the nodes per second from the last search.
long long Engine::lastNodesPerSecond() const {
    return static_cast<long long>(lastNps);
}

// Runs negamax search with alpha-beta pruning.
int Engine::negamax(Board& board, int depth, int alpha, int beta, Color side) {
    nodes++;
    if (depth == 0) {
        return evaluate(board, side);
    }

    const std::vector<Move> moves = board.generateLegalMoves();
    if (moves.empty()) {
        return board.inCheck(side) ? -100000 : 0;
    }

    const Color nextSide = (side == WHITE) ? BLACK : WHITE;
    for (size_t i = 0; i < moves.size(); ++i) {
        const Move& m = moves[i];
        Board::MoveRecord record = board.makeMove(m);
        const int score = -negamax(board, depth - 1, -beta, -alpha, nextSide);
        board.undoMove(record);

        if (score > alpha) {
            alpha = score;
            if (alpha >= beta) {
                break;
            }
        }
    }

    return alpha;
}

static const int pawnTable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, -20, -20, 10, 10, 5,
    5, -5, -10, 0, 0, -10, -5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, 5, 10, 25, 25, 10, 5, 5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0, 0, 0, 0, 0, 0, 0, 0
};

static const int knightTable[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

static const int bishopTable[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20
};

static const int rookTable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0
};

static const int queenTable[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20
};

static const int kingTable[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20
};

// Returns a piece-square table value for a piece on a square.
int Engine::pstValue(PieceType type, Color color, int x, int y) const {
    const int index = (color == WHITE) ? (y * 8 + x) : ((7 - y) * 8 + x);
    switch (type) {
        case PAWN:
            return pawnTable[index];
        case KNIGHT:
            return knightTable[index];
        case BISHOP:
            return bishopTable[index];
        case ROOK:
            return rookTable[index];
        case QUEEN:
            return queenTable[index];
        case KING:
            return kingTable[index];
        default:
            return 0;
    }
}

// Evaluates the board material from the given side's perspective.
int Engine::evaluate(const Board& board, Color side) const {
    int score = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Piece& p = board.board[y][x];
            int value = 0;
            switch (p.type) {
                case PAWN:
                    value = 100;
                    break;
                case KNIGHT:
                    value = 320;
                    break;
                case BISHOP:
                    value = 330;
                    break;
                case ROOK:
                    value = 500;
                    break;
                case QUEEN:
                    value = 900;
                    break;
                case KING:
                    value = 20000;
                    break;
                default:
                    value = 0;
                    break;
            }

            value += pstValue(p.type, p.color, x, y);
            if (p.color == WHITE) {
                score += value;
            } else if (p.color == BLACK) {
                score -= value;
            }
        }
    }

    return (side == WHITE) ? score : -score;
}
