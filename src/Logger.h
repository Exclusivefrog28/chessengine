#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <thread>

struct BufferNode {
	BufferNode* next;
	std::string msg;
};


class Logger {
public:
	void start();
	void end();

	void log(const std::string&message) const;

	Logger();
	~Logger();
private:
	mutable BufferNode* head;
	mutable BufferNode* tail;

	void threadFunc() const;
	bool processNode() const;
	std::thread processingThread;
	bool stop;
};



#endif //LOGGER_H
