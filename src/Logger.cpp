//
// Created by Martin on 08.05.2020.
//

#include "Logger.h"
#include <fstream>
#include <ctime>
#include <chrono>

Logger::Logger() {
	stream.open(L"../logs/" + fileNameByDate() + L".log", std::ios::app);

	if (stream.fail()) {
		throw std::iostream::failure("Cannot open file: pokus.log");
	}
}

std::wstring Logger::fileNameByDate() {
	time_t rawTime;
	struct tm* timeInfo;
	wchar_t buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	wcsftime(buffer, 80, L"%d-%m-%Y %H-%M-%S", timeInfo);
	return std::wstring(buffer);
}

std::wstring Logger::timeFormat() {
	time_t rawTime;
	struct tm* timeInfo;
	wchar_t buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	wcsftime(buffer, 80, L"%d.%m.%Y %H:%M:%S", timeInfo);
	return std::wstring(buffer);
}

void Logger::fatal(std::wstring text) {
	stream << "[FATAL - " << timeFormat() << "] " << text << "\n";
}

void Logger::error(std::wstring text) {
	stream << "[ERROR - " << timeFormat() << "] " << text << "\n";
}

void Logger::warn(std::wstring text) {
	stream << "[WARN - " << timeFormat() << "] " << text << "\n";
}

void Logger::info(std::wstring text) {
	stream << "[INFO - " << timeFormat() << "] " << text << "\n";
}

void Logger::debug(std::wstring text) {
	stream << "[DEBUG - " << timeFormat() << "] " << text << "\n";
}

void Logger::trace(std::wstring text) {
	stream << "[TRACE - " << timeFormat() << "] " << text << "\n";
}