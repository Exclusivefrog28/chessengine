#include "Logger.h"

#include <iostream>

void Logger::start() {
	stop = false;
	processingThread = std::thread(&Logger::threadFunc, this);
}

void Logger::end() {
	stop = true;
	processingThread.join();
}

void Logger::log(const std::string&message) const {
	auto* newNode = new BufferNode(nullptr, message);
	tail->next = newNode;
	tail = newNode;
}

Logger::Logger() {
	this->head = new BufferNode();
	this->tail = this->head;
	stop = false;
}

Logger::~Logger() {
	delete this->head;
}

void Logger::threadFunc() const {
	while (!stop) {
		// Keep processing the buffer, wait a bit when it's empty
		if(!processNode()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	// Empty the buffer
	while (processNode()){}
}

bool Logger::processNode() const {
	if (head->next != nullptr) {
		BufferNode* newHead = head->next;

		std::cout << newHead->msg;

		delete head;
		head = newHead;
		return true;
	}
	return false;
}
