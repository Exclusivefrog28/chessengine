#include "CLI.h"
#include <iostream>
#include <sstream>
#include <thread>

#include "MoveGenerator.h"
#include "Search.h"
#include "MoveParser.h"

namespace Interface {
    void CLI::start() {
        board = ChessBoard();
        Instruction currentInstruction;
        currentInstruction.command = invalid;
        while (currentInstruction.command != quit) {
            std::string input;
            std::getline(std::cin, input);
            currentInstruction = interpret(input);

            std::unique_lock lk(m);
            cv.wait(lk, [this] { return ready; });
            ready = false;
            std::thread thread(&CLI::handleInstruction, this, currentInstruction);
            thread.detach();
        }
    }

    Instruction CLI::interpret(const std::string &string) {
        std::vector<std::string> strings;
        std::stringstream ss(string);
        std::string item;

        while (getline(ss, item, ' ')) {
            strings.push_back(item);
        }

        const auto it = commandMap.find(strings[0]);
        const Command command = it != commandMap.end() ? it->second : invalid;

        return Instruction(command, std::vector(strings.begin() + 1, strings.end()));
    }

    void CLI::handleInstruction(const Instruction &instr) {
        switch (instr.command) {
            case uci:
                std::cout << "id name " << "EngineName" << std::endl;
                std::cout << "id author " << "Exclusivefrog28" << std::endl;
                std::cout << "uciok" << std::endl;
                break;
            case isready:
                std::cout << "readyok" << std::endl;
                break;
            case ucinewgame:
                ready = false;
                Search::tt.clear();
                board = ChessBoard();
                ready = true;
                break;
            case position: {
                int startIndex = 1;
                if (instr.args[0] == "startpos") {
                    board.setStartingPosition();
                    Search::tt.loadOpenings(board);
                } else if (instr.args[0] == "fen") {
                    const std::string fen = instr.args[1] + " " +
                                            instr.args[2] + " " +
                                            instr.args[3] + " " +
                                            instr.args[4] + " " +
                                            instr.args[5] + " " +
                                            instr.args[6];
                    startIndex = 7;
                    board.setPosition(fen);
                }
                if (instr.args.size() > startIndex && instr.args[startIndex] == "moves") {
                    for (int i = startIndex + 1; i < instr.args.size(); ++i) {
                        board.makeMove(parseMove(instr.args[i], board));
                    }
                }
                break;
            }
            case go: {
                int timeOut = 3000;

                if (!instr.args.empty()) {
                    const std::string arg = instr.args[0];
                    if (arg == "movetime") {
                        timeOut = std::stoi(instr.args[1]);
                    } else if (arg == "perft") {
                        const uint64_t hashBefore = board.hashCode;
                        const int depth = std::stoi(instr.args[1]);
                        const uint64_t nodes = MoveGenerator::perft(depth, board);
                        std::cout << nodes << std::endl;
                        if (board.hashCode != hashBefore) {
                            std::cout << "Hash mismatch" << std::endl;
                        }
                        break;
                    }
                }

                const Move bestMove = Search::search(board, timeOut);
                board.makeMove(bestMove);

                std::cout << "bestmove " << bestMove << std::endl;

                break;
            }
            default:
                break;
        }
        {
            std::lock_guard lk(m);
            ready = true;
        }
        cv.notify_one();
    }
}
