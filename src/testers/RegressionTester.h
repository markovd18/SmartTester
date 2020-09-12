#ifndef SMARTTESTER_REGRESSIONTESTER_H
#define SMARTTESTER_REGRESSIONTESTER_H

#include <rtl/Dynamic_Library.h>
#include "../utils/Logger.h"

class RegressionTester {
private:
    std::wstring config_filepath;
    void loadConfig();
    std::vector<std::vector<std::string>> setLogVector(const std::string& cLog);
    bool compareLines(std::vector<std::string> log, std::vector<std::string> result);
    void printLines(const std::vector<std::vector<std::string>>& errorResult);
    void printAndEmptyErrors(const refcnt::Swstr_list& errors);
    void printOneLine(const std::vector<std::string>& line);
public:
    Logger& logger = Logger::GetInstance();
    explicit RegressionTester(const std::wstring& config_filepath);
    HRESULT compareLogs(const std::string& cLog, const std::string& rLog);
};
#endif //SMARTTESTER_UNITTESTER_H
