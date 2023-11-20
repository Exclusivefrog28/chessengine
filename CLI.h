#ifndef INTERFACE_H
#define INTERFACE_H
#include <string>
#include <unordered_map>
#include <vector>

#include "ChessBoard.h"

//Universal Chess Interface implementation
namespace Interface {
	enum Command {
		invalid,
		uci,
		isready,
		ucinewgame,
		position,
		go,
		stop,
		ponderhit,
		quit
	};

	struct Instruction {
		Command command;
		std::vector<std::string> args;
	};

	const inline std::unordered_map<std::string, Command> commandMap = {
		{"quit", quit},
		{"uci", uci},
		{"isready", isready},
		{"ucinewgame", ucinewgame},
		{"position", position},
		{"go", go},
		{"stop", stop},
		{"ponderhit", ponderhit}
	};

	class CLI {
	public:
		void start();

	private:
		ChessBoard board;

		static Instruction interpret(const std::string&string);

		static Move parseMove(const std::string&string, const ChessBoard&board);

		void handleInstruction(const Instruction&instr);
	};
}

#endif //INTERFACE_H
