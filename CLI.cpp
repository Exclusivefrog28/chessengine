#include "CLI.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace Interface {
    void CLI::start() {
        std::cout << "Hello Chess!\n";
        std::cout << "Waiting for next command...\n";
        Instruction currentInstruction;
        while (currentInstruction.command != quit) {
            std::string input;
            std::getline(std::cin, input);
            currentInstruction = interpret(input);
            handleInstruction(currentInstruction);
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

    void CLI::handleInstruction(const Instruction&instr) {
        std::cout << "Received command: " << instr.command << '\n';
    }
}
