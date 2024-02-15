#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <thread>
#include <vector>

enum MessageType {
	LOG = 0,
	DATA = 1
};

struct Node {
	Node* next;
	MessageType type;
};

struct MessageNode : Node {
	std::string msg;
};

struct DataNode : Node {
	std::string name;
	int value;
};

class Logger {
public:
	void start();

	void end();

	void log(const std::string&message) const;

	void sendData(const std::string&name, int value) const;

	Logger();

	~Logger();

private:
	mutable Node* head;
	mutable Node* tail;

	void threadFunc() const;

	bool processNode() const;

	std::thread processingThread;
	bool stop;
};


#endif //LOGGER_H
