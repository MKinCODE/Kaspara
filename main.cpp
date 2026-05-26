#include "board.h"

#include <iostream>

// Entry point: initializes and prints the starting board position.
int main() {
    Board board;
    board.reset();
    board.print();
    std::cout << "Kaspara Chess Engine v0.1 — Type your move to begin\n";
    return 0;
}
