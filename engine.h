#ifndef KASPARA_ENGINE_H
#define KASPARA_ENGINE_H

#include "board.h"

class Engine {
public:
    // Creates an engine with a maximum search depth.
    explicit Engine(int depth = 4);

    // Finds the best move for the current board position.
    Move findBestMove(Board& board);

    // Returns the depth used for the last search.
    int lastDepth() const;

    // Returns the number of nodes evaluated in the last search.
    long long lastNodesEvaluated() const;

    // Returns the nodes per second from the last search.
    long long lastNodesPerSecond() const;

private:
    int maxDepth;
    long long nodes;
    long long lastNodes;
    double lastNps;

    // Runs negamax search with alpha-beta pruning.
    int negamax(Board& board, int depth, int alpha, int beta, Color side);

    // Returns a piece-square table value for a piece on a square.
    int pstValue(PieceType type, Color color, int x, int y) const;

    // Evaluates the board material from the given side's perspective.
    int evaluate(const Board& board, Color side) const;
};

#endif
