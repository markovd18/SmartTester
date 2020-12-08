//
// Created by Martin on 08.05.2020.
//

#include "../Logger.h"
#include <fstream>
#include <ctime>

#if __has_include(<filesystem>)
	#include <filesystem>
	namespace fs = std::filesystem;
#else
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#endif

Logger::Logger() {
	fs::create_directory("../../logs");
	stream.open("../../logs/" + fileNameByDate() + ".log", std::ios::app);
}

std::string Logger::fileNameByDate() {
	time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(buffer, 80, "%d-%m-%Y %H-%M-%S", timeInfo);
	return std::string(buffer);
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

void Logger::error(const std::wstring& text) {
	stream << "[ERROR - " << timeFormat() << "] " << text << "\n";
}

void Logger::warn(const std::wstring& text) {
	stream << "[WARN - " << timeFormat() << "] " << text << "\n";
}

void Logger::info(const std::wstring& text) {
	stream << "[INFO - " << timeFormat() << "] " << text << "\n";
}

void Logger::debug(const std::wstring& text) {
	stream << "[DEBUG - " << timeFormat() << "] " << text << "\n";
}

void Logger::trace(const std::wstring& text) {
	stream << "[TRACE - " << timeFormat() << "] " << text << "\n";
}

Logger& Logger::GetInstance() {
	static Logger instance;
	return instance;
}