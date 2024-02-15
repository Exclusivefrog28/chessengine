#include "Logger.h"

#include <iostream>
#include <format>

#ifdef wasm
#include "emscripten.h"
#include "emscripten/threading.h"
#endif

void Logger::start() {
	stop = false;
	processingThread = std::thread(&Logger::threadFunc, this);
}

void Logger::end() {
	stop = true;
	processingThread.join();
}

void Logger::log(const std::string&message) const {
	auto* newNode = new MessageNode();
	newNode->type = LOG;
	newNode->msg = message;
	tail->next = newNode;
	tail = newNode;
}

void Logger::sendData(const std::string&name, const int value) const {
	auto* newNode = new DataNode();
	newNode->type = DATA;
	newNode->name = name;
	newNode->value = value;
	tail->next = newNode;
	tail = newNode;
}

Logger::Logger() {
	this->head = new MessageNode();
	this->tail = this->head;
	stop = false;
}

Logger::~Logger() {
	delete this->head;
}

void Logger::threadFunc() const {
	while (!stop) {
		// Keep processing the buffer, wait a bit when it's empty
		if (!processNode()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	// Empty the buffer
	while (processNode()) {
	}
}

bool Logger::processNode() const {
	if (head->next != nullptr) {
		Node* newHead = head->next;

		if (newHead->type == LOG) std::cout << reinterpret_cast<MessageNode *>(newHead)->msg;
#ifdef wasm
		else
			MAIN_THREAD_ASYNC_EM_ASM(
			postMessage({data: [$1], name: UTF8ToString($0)})
			, reinterpret_cast<DataNode*>(newHead)->name.c_str(), reinterpret_cast<DataNode*>(newHead)->value);

#endif

		delete head;
		head = newHead;
		return true;
	}
	return false;
}
