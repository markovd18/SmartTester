//
// Created by Martin on 08.05.2020.
//

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>

    /**
	Class Logger is used to simplify logging of runtime information into a file.
    */
    class Logger {
    public:
        ~Logger() {
            m_stream.close();
        }

        void error(const std::wstring &text);

        void warn(const std::wstring &text);

        void info(const std::wstring &text);

        void debug(const std::wstring &text);

        static Logger &getInstance();

    private:
        Logger();

        void log(const std::wstring& text, const std::wstring& level);

        std::wofstream m_stream;
    };

    std::string currentTime();

    std::string currentDate();

    std::string dateTimeInFormat(const char* format);
#endif //LOGGER_H
