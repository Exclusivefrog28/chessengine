#ifndef LOGGER_H
#define LOGGER_H
#include <condition_variable>
#include <string>
#include <thread>
#include <fstream>
#include <semaphore>

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

	void log(const std::string& message) const;

	void logToFile(const std::string& message) const;

	void sendInt(const std::string& name, int value) const;

	void sendString(const std::string& name, const std::string& value) const;

	Logger();

	~Logger();

private:
	mutable Node* head;
	mutable Node* tail;
	std::ofstream logFile;

	void threadFunc();

	bool processNode();

	std::thread processingThread;

	mutable std::mutex m;
	mutable std::condition_variable cv;
	mutable bool empty{};
	mutable bool stop;
};

#endif //LOGGER_H
