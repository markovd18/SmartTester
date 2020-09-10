//
// Created by Martin on 08.05.2020.
//

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>

/**
	Class Logger is used to simplify logging of runtime information into a file.
*/
class Logger {
public:
	Logger();

	~Logger() {
		stream.close();
	}

	void error(const std::wstring& text);
	void warn(const std::wstring& text);
	void info(const std::wstring& text);
	void debug(const std::wstring& text);
	void trace(const std::wstring& text);
	static Logger& GetInstance();
private:
	std::wofstream stream;
	std::string fileNameByDate();
	std::wstring timeFormat();
};

#endif //LOGGER_H
