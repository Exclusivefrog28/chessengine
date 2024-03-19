#include "Logger.h"

#include <iostream>

#ifdef wasm
#include "emscripten.h"
#include "emscripten/threading.h"
#endif

void Logger::start() {
	stop = false;
	empty = true;
	processingThread = std::thread(&Logger::threadFunc, this);
#ifdef logtofile
	logFile.open("log.txt");
#endif
}

void Logger::end() {
	stop = true;
	processingThread.join();
#ifdef logtofile
	logFile.close();
#endif
}

void Logger::log(const std::string&message) const {
	auto* newNode = new MessageNode();
	newNode->type = LOG;
	newNode->msg = message;
	tail->next = newNode;
	tail = newNode; {
		std::lock_guard lk(m);
		empty = false;
	}
	cv.notify_one();
}

void Logger::logToFile(const std::string&message) const {
	// Since the function doesn't do anything with the flag disabled the compiler should technically remove it and all the calls to it
#ifdef logtofile
	auto* newNode = new MessageNode();
	newNode->type = TOFILE;
	newNode->msg = message;
	tail->next = newNode;
	tail = newNode; {
		std::lock_guard lk(m);
		empty = false;
	}
	cv.notify_one();
#endif
}

void Logger::sendInt(const std::string&name, const int value) const {
	// Since the function doesn't do anything with the flag disabled the compiler should technically remove it and all the calls to it
#ifdef wasm
	auto* newNode = new IntNode();
	newNode->type = INT;
	newNode->name = name;
	newNode->value = value;
	tail->next = newNode;
	tail = newNode;
	{
		std::lock_guard lk(m);
		empty = false;
	}
	cv.notify_one();
#endif
}

void Logger::sendString(const std::string&name, const std::string&value) const {
	// Since the function doesn't do anything with the flag disabled the compiler should technically remove it and all the calls to it
#ifdef wasm
	auto* newNode = new StringNode();
	newNode->type = STRING;
	newNode->name = name;
	newNode->value = value;
	tail->next = newNode;
	tail = newNode;
	{
		std::lock_guard lk(m);
		empty = false;
	}
	cv.notify_one();
#endif
}


Logger::Logger() {
	this->head = new MessageNode();
	this->tail = this->head;
	stop = false;
	empty = true;
}

Logger::~Logger() {
	delete this->head;
}

void Logger::threadFunc() {
	while (!stop) {
		// Keep processing the buffer, wait a bit when it's empty
		if (!processNode()) {
			std::unique_lock lk(m);
			empty = true;
			cv.wait(lk, [this] { return !empty || stop; });
			lk.unlock();
		}
	}
	// Empty the buffer
	while (processNode()) {
	}
}

bool Logger::processNode() {
	if (head->next != nullptr) {
		Node* newHead = head->next;

#ifndef wasm
		if (newHead->type == LOG) {
			std::cout << reinterpret_cast<MessageNode *>(newHead)->msg;
		}
#ifdef logtofile
		else if (newHead->type == TOFILE) {
			logFile << reinterpret_cast<MessageNode *>(newHead)->msg;
		}
#endif
#endif

#ifdef wasm
		switch (newHead->type) {
			case LOG: {
				MAIN_THREAD_EM_ASM(
					postMessage({data: [UTF8ToString($0)], name: "log"})
					, reinterpret_cast<MessageNode *>(newHead)->msg.c_str());
				break;
			}
			case STRING: {
				MAIN_THREAD_EM_ASM(
					postMessage({data: [UTF8ToString($1)], name: UTF8ToString($0)})
					, reinterpret_cast<StringNode *>(newHead)->name.c_str(), reinterpret_cast<StringNode *>(newHead)->value.c_str());
				break;
			}
			case INT: {
				MAIN_THREAD_EM_ASM(
					postMessage({data: [$1], name: UTF8ToString($0)})
					, reinterpret_cast<IntNode *>(newHead)->name.c_str(), reinterpret_cast<IntNode *>(newHead)->value);
				break;
			}
			default: break;
		}
#endif

		delete head;
		head = newHead;
		return true;
	}
	return false;
}
