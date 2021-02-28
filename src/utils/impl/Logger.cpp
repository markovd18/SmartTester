//
// Author: markovd@students.zcu.cz, marstr@students.zcu.cz
//

#include <fstream>
#include <ctime>

#include <rtl/FilesystemLib.h>
#include <string>
#include <utils/string_utils.h>
#include "../Logger.h"

    Logger::Logger() {
        filesystem::create_directory("../../logs");
        m_stream.open("../../logs/" + currentDate() + ".log", std::ios::app);
    }

    void Logger::error(const std::wstring &text) {
        log(text, L"ERROR");
    }

    void Logger::warn(const std::wstring &text) {
        log(text, L"WARN");
    }

    void Logger::info(const std::wstring &text) {
        log(text, L"INFO");
    }

    void Logger::debug(const std::wstring &text) {
#ifndef NDEBUG
        log(text, L"DEBUG");
#endif
    }

    void Logger::log(const std::wstring &text, const std::wstring &level) {
        std::wstring logText = Widen_Char(currentTime().c_str()) + L" " + level + L"\t" + text + L'\n';
        m_stream << logText;
        m_stream.flush();
    }

    Logger &Logger::getInstance() {
        static Logger instance;
        return instance;
    }

    std::string currentTime() {
        return dateTimeInFormat("%Y-%m-%d %H:%M:%S");
    }

    std::string currentDate() {
        return dateTimeInFormat("%Y-%m-%d");
    }

    std::string dateTimeInFormat(const char* format) {
        time_t rawTime;
        struct tm *timeInfo;
        char buffer[80];

        time(&rawTime);
        timeInfo = localtime(&rawTime);

        strftime(buffer, 80, format, timeInfo);
        return std::string(buffer);
    }
