#ifndef INTERFACE_H
#define INTERFACE_H
#include <string>
#include <unordered_map>
#include <vector>
#include <condition_variable>

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

        std::mutex m;
        std::condition_variable cv;
        bool ready = true;

		static Instruction interpret(const std::string&string);

		void handleInstruction(const Instruction&instr);
	};
}

#endif //INTERFACE_H
