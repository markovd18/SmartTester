//
// Created by Martin on 08.05.2020.
//

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>

/**
	Class Logger is used to simplify logging of runtime informations into a file.
*/
class Logger {
public:
	Logger();

	~Logger() {
		stream.close();
	}

	void fatal(std::wstring text);
	void error(std::wstring text);
	void warn(std::wstring text);
	void info(std::wstring text);
	void debug(std::wstring text);
	void trace(std::wstring text);
	static Logger& GetInstance();
private:
	std::wofstream stream;
	std::wstring fileNameByDate();
	std::wstring timeFormat();
};

#endif //LOGGER_H
