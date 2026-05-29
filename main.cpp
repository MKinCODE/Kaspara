#include "board.h"
#include "engine.h"

#include <iostream>
#include <string>

// Converts a move to UCI notation.
std::string moveToUci(const Move& m) {
    std::string uci;
    uci += static_cast<char>('a' + m.fromX);
    uci += static_cast<char>('1' + m.fromY);
    uci += static_cast<char>('a' + m.toX);
    uci += static_cast<char>('1' + m.toY);
    if (m.promotion != EMPTY) {
        char promo = 'q';
        switch (m.promotion) {
            case ROOK:
                promo = 'r';
                break;
            case BISHOP:
                promo = 'b';
                break;
            case KNIGHT:
                promo = 'n';
                break;
            default:
                promo = 'q';
                break;
        }
        uci += promo;
    }
    return uci;
}

// Entry point: initializes and prints the starting board position.
int main() {
    Board board;
    board.reset();
    board.print();
    const std::vector<Move> moves = board.generatePseudoMoves();
    std::cout << "Pseudo moves: " << moves.size() << "\n";
    const std::vector<Move> legalMoves = board.generateLegalMoves();
    std::cout << "Legal moves: " << legalMoves.size() << "\n";
    Engine engine(4);
    const Move bestMove = engine.findBestMove(board);
    std::cout << "Best move: " << moveToUci(bestMove) << "\n";
    std::cout << "Nodes evaluated: " << engine.lastNodesEvaluated() << "\n";
    std::cout << "Nodes per second: " << engine.lastNodesPerSecond() << "\n";
    std::cout << "Kaspara Chess Engine v0.1 — Type your move to begin\n";
    return 0;
}
