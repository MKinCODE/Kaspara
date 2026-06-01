#include "board.h"
#include "engine.h"

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

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

// Parses a UCI move string into a Move object.
bool parseUciMove(const std::string& input, Move& move) {
    if (input.size() < 4 || input.size() > 5) {
        return false;
    }
    const char f1 = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
    const char r1 = input[1];
    const char f2 = static_cast<char>(std::tolower(static_cast<unsigned char>(input[2])));
    const char r2 = input[3];
    if (f1 < 'a' || f1 > 'h' || f2 < 'a' || f2 > 'h' || r1 < '1' || r1 > '8' || r2 < '1' ||
        r2 > '8') {
        return false;
    }

    move.fromX = f1 - 'a';
    move.fromY = r1 - '1';
    move.toX = f2 - 'a';
    move.toY = r2 - '1';
    move.promotion = EMPTY;
    move.isEnPassant = false;
    move.isCastle = false;

    if (input.size() == 5) {
        const char promo = static_cast<char>(std::tolower(static_cast<unsigned char>(input[4])));
        switch (promo) {
            case 'q':
                move.promotion = QUEEN;
                break;
            case 'r':
                move.promotion = ROOK;
                break;
            case 'b':
                move.promotion = BISHOP;
                break;
            case 'n':
                move.promotion = KNIGHT;
                break;
            default:
                return false;
        }
    }

    return true;
}

// Runs the main game loop for human vs engine.
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    Board board;
    board.reset();
    Engine engine(4);

    std::cout << "Kaspara Chess Engine — Human (White) vs Engine (Black)\n";

    while (true) {
        board.print();
        const std::vector<Move> legalMoves = board.generateLegalMoves();
        if (legalMoves.empty()) {
            if (board.inCheck(board.sideToMove)) {
                const char* winner = (board.sideToMove == WHITE) ? "Black" : "White";
                std::cout << "Checkmate! " << winner << " wins.\n";
            } else {
                std::cout << "Stalemate. Draw.\n";
            }
            break;
        }

        if (board.sideToMove == WHITE) {
            while (true) {
                std::cout << "Your move (e.g. e2e4, e7e8q for promotion): ";
                std::string input;
                if (!(std::cin >> input)) {
                    return 0;
                }
                if (input == "quit") {
                    return 0;
                }

                Move parsedMove;
                if (!parseUciMove(input, parsedMove)) {
                    std::cout << "Illegal move. Try again.\n";
                    continue;
                }

                const Move* chosenMove = nullptr;
                for (size_t i = 0; i < legalMoves.size(); ++i) {
                    const Move& m = legalMoves[i];
                    if (m.fromX == parsedMove.fromX && m.fromY == parsedMove.fromY &&
                        m.toX == parsedMove.toX && m.toY == parsedMove.toY &&
                        m.promotion == parsedMove.promotion) {
                        chosenMove = &m;
                        break;
                    }
                }

                if (!chosenMove) {
                    std::cout << "Illegal move. Try again.\n";
                    continue;
                }

                board.makeMove(*chosenMove);
                break;
            }
        } else {
            std::cout << "Engine is thinking...\n";
            const Move bestMove = engine.findBestMove(board);
            board.makeMove(bestMove);
            std::cout << "Engine plays: " << moveToUci(bestMove) << "\n";
            std::cout << "Depth: " << engine.lastDepth() << " | Nodes: " << engine.lastNodesEvaluated()
                      << " | Speed: " << engine.lastNodesPerSecond() << " nodes/sec\n";
        }
    }

    return 0;
}
