#include "CLI.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <thread>

#include "MoveGenerator.h"
#include "Search.h"

namespace Interface {
	void CLI::start() {
		board.hashCodes.initialize();
		std::cout << "Hello Chess!" << std::endl;
		std::cout << "Waiting for next command..." << std::endl;
		Instruction currentInstruction;
		while (currentInstruction.command != quit) {
			std::string input;
			std::getline(std::cin, input);
			currentInstruction = interpret(input);

			std::thread thread(&CLI::handleInstruction, this, currentInstruction);
			thread.detach();
		}
	}

	Instruction CLI::interpret(const std::string&string) {
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

	Move CLI::parseMove(const std::string&string, const ChessBoard&board) {
		const int startPos = Util::stringToPosition(string.substr(0, 2));
		const int endPos = Util::stringToPosition(string.substr(2, 2));
		Type promotionType = EMPTY;
		MoveFlag flag = QUIET;

		if (string.length() > 4) {
			promotionType = Util::charToPiece(string[4]).type;
		}
		const Square&startSquare = board.squares[startPos];
		const Square&endSquare = board.squares[endPos];
		if (endSquare.type != EMPTY && endSquare.color != board.sideToMove) {
			flag = static_cast<MoveFlag>(endSquare.type);
		}
		else {
			if (startSquare.type == PAWN) {
				if (startPos / 8 == 2 && endPos / 8 == 4 || startPos / 8 == 7 && endPos / 8 == 5) {
					flag = DOUBLEPAWNPUSH;
				}
				if (abs(startPos - endPos) == 7 || abs(startPos - endPos) == 9) {
					flag = ENPASSANT;
				}
			}
			else {
				if (startSquare.type == KING) {
					if (startPos == 4 && startSquare.color == BLACK) {
						if (endPos == 2) flag = CASTLEQUEENSIDE;
						else if (endPos == 6) flag = CASTLEKINGSIDE;
					}
					if (startPos == 60 && startSquare.color == WHITE) {
						if (endPos == 58) flag = CASTLEQUEENSIDE;
						else if (endPos == 62) flag = CASTLEKINGSIDE;
					}
				}
			}
		}

		return Move(startPos, endPos, promotionType, flag, board.sideToMove);
	}


	void CLI::handleInstruction(const Instruction&instr) {
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
				Search::tt = TranspositionTable();
				break;
			case position: {
				int startIndex = 1;
				if (instr.args[0] == "startpos") board.setStartingPosition();
				else {
					const std::string fen = instr.args[0] + " " +
					                        instr.args[1] + " " +
					                        instr.args[2] + " " +
					                        instr.args[3] + " " +
					                        instr.args[4] + " " +
					                        instr.args[5];
					startIndex = 6;
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

				const std::string arg = instr.args[0];
				if (arg == "movetime") {
					timeOut = std::stoi(instr.args[1]);
				}
				else if (arg == "perft") {
					const int depth = std::stoi(instr.args[1]);
					const unsigned long long nodes = MoveGenerator::perft(depth, board);
					std::cout << nodes << std::endl;
					break;
				}

				const Move bestMove = Search::search(board, timeOut);
				std::cout << board.fen() << std::endl;

				const std::string promotion = (bestMove.promotionType != EMPTY) ? Util::pieceToString(bestMove.promotionType, BLACK) : "";

				bool legal = false;
				for (const auto moves = MoveGenerator::pseudoLegalMoves(board); const Move&move: moves) {
					if (move == bestMove) {
						legal = true;
						break;
					}
				}
				if (legal) {
					legal = MoveGenerator::isLegalMove(board, bestMove);
				}

				if (!legal) {
					std::cout << Util::positionToString(bestMove.start) << Util::positionToString(bestMove.end) << " " << bestMove.flag << std::endl;
				}
				else {
					std::cout << "bestmove " << Util::positionToString(bestMove.start) << Util::positionToString(bestMove.end) << promotion << std::endl;
				}

				break;
			}
			default:
				break;
		}
	}
}
