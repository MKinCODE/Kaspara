#include "board.h"

#include <iostream>

// Entry point: initializes and prints the starting board position.
int main() {
    Board board;
    board.reset();
    board.print();
    const std::vector<Move> moves = board.generatePseudoMoves();
    std::cout << "Pseudo moves: " << moves.size() << "\n";
    std::cout << "Kaspara Chess Engine v0.1 — Type your move to begin\n";
    return 0;
}
