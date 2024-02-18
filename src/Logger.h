#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <thread>
#include <fstream>

enum MessageType {
	LOG = 0,
	INT = 1,
	STRING = 2,
	TOFILE = 3
};

struct Node {
	Node* next;
	MessageType type;
};

struct MessageNode : Node {
	std::string msg;
};

struct StringNode : Node {
	std::string name;
	std::string value;
};

struct IntNode : Node {
	std::string name;
	int value;
};

class Logger {
public:
	void start();

	void end();

	void log(std::string message) const;

	void logToFile(std::string message) const;

	void sendInt(std::string name, int value) const;

	void sendString(std::string name, std::string value) const;

	Logger();

	~Logger();

private:
	mutable Node* head;
	mutable Node* tail;
	mutable std::ofstream logFile;

	void threadFunc() const;

	bool processNode() const;

	std::thread processingThread;
	bool stop;
};

#endif //LOGGER_H
