#ifndef SMARTTESTER_REGRESSIONTESTER_H
#define SMARTTESTER_REGRESSIONTESTER_H

#include <rtl/Dynamic_Library.h>
#include "../utils/Logger.h"

class RegressionTester {
private:
    std::wstring config_filepath;
    std::string resultLog;

    void loadConfig();
    std::vector<std::vector<std::string>> readLogFile(const std::string& logPath);
    bool compareLines(std::vector<std::string> resultLogLine, std::vector<std::string> referenceLogLine);
    void printLines(const std::vector<std::vector<std::string>>& errorResult);
    void printAndEmptyErrors(const refcnt::Swstr_list& errors);
    void printOneLine(const std::vector<std::string>& line);
public:
    Logger& logger = Logger::GetInstance();
    explicit RegressionTester(std::wstring  config_filepath);
    HRESULT compareLogs(const std::string& referenceLog);
};
#endif //SMARTTESTER_UNITTESTER_H
