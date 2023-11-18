#ifndef INTERFACE_H
#define INTERFACE_H
#include <string>
#include <unordered_map>
#include <vector>

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
        std::vector<std::string> arguments;
    };

    const inline std::unordered_map<std::string, Command> commandMap = {
        {"quit", Command::quit},
        {"uci", Command::uci},
        {"isready", Command::isready},
        {"ucinewgame", Command::ucinewgame},
        {"position", Command::position},
        {"go", Command::go},
        {"stop", Command::stop},
        {"ponderhit", Command::ponderhit}
    };

    class CLI {
    public:
        static void start();

    private:
        static Instruction interpret(const std::string&string);

        static void handleInstruction(const Instruction&instr);
    };
}

#endif //INTERFACE_H
